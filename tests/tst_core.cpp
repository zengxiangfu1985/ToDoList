#include "../src/core/task_repository.h"
#include "../src/core/priority_engine.h"
#include "../src/core/behavior_learning_engine.h"
#include "../src/core/ai/llm_provider.h"
#include "../src/utils/portable_settings.h"

#include <QtTest>
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QElapsedTimer>
#include <QSettings>

class TestCore : public QObject
{
    Q_OBJECT

private slots:
    void ut_S01_01_addTask();
    void ut_S01_02_emptyTitle();
    void ut_S02_01_ruleScorePerformance();
    void ut_S02_02_ruleTop3Fallback();
    void ut_S03_01_factoryCreatesAll();
    void ut_S03_02_deepseekRequiresApiKey();
    void ut_S03_03_settingsRoundTrip();
    void ut_S05_01_updateQuadrant();
    void ut_S05_02_learningWeightsChange();
    void ut_S05_03_scoringUsesWeights();
};

void TestCore::ut_S01_01_addTask()
{
    QTemporaryDir tmp;
    TaskRepository repo;
    QString err;
    QVERIFY(repo.open(tmp.path() + QStringLiteral("/t.db"), &err));

    TaskItem task;
    task.title = QStringLiteral("Write report");
    task.quadrant = EisenhowerQuadrant::Q2_NotUrgentImportant;
    QVERIFY(repo.addTask(&task, &err));
    QCOMPARE(repo.allTasks().size(), 1);
}

void TestCore::ut_S01_02_emptyTitle()
{
    QTemporaryDir tmp;
    TaskRepository repo;
    QString err;
    QVERIFY(repo.open(tmp.path() + QStringLiteral("/t.db"), &err));

    TaskItem task;
    task.title = QStringLiteral("   ");
    QVERIFY(!repo.addTask(&task, &err));
}

void TestCore::ut_S02_01_ruleScorePerformance()
{
    QVector<TaskItem> tasks;
    for (int i = 0; i < 50; ++i) {
        TaskItem t;
        t.id = i;
        t.title = QStringLiteral("Task %1").arg(i);
        t.quadrant = static_cast<EisenhowerQuadrant>((i % 4) + 1);
        t.dueAt = QDateTime::currentDateTimeUtc().addDays(i % 7);
        tasks.append(t);
    }
    QElapsedTimer timer;
    timer.start();
    PriorityEngine::applyRuleScores(tasks);
    QVERIFY(timer.elapsed() < 200);
}

void TestCore::ut_S03_01_factoryCreatesAll()
{
    const QList<LlmProviderType> types = {
        LlmProviderType::Ollama,
        LlmProviderType::DeepSeek,
        LlmProviderType::Kimi,
        LlmProviderType::CustomOpenAI
    };
    for (LlmProviderType type : types) {
        const LlmConfig cfg = LlmProviderFactory::defaultConfig(type);
        auto p = LlmProviderFactory::create(cfg);
        QVERIFY(p != nullptr);
    }
}

void TestCore::ut_S03_02_deepseekRequiresApiKey()
{
    LlmConfig cfg = LlmProviderFactory::defaultConfig(LlmProviderType::DeepSeek);
    cfg.apiKey.clear();
    auto p = LlmProviderFactory::create(cfg);
    QVERIFY(p != nullptr);
    QVERIFY(!p->isConfigured());
}

void TestCore::ut_S02_02_ruleTop3Fallback()
{
    QVector<TaskItem> tasks;
    for (int i = 0; i < 5; ++i) {
        TaskItem t;
        t.id = i + 1;
        t.title = QStringLiteral("Task %1").arg(i);
        t.quadrant = EisenhowerQuadrant::Q1_UrgentImportant;
        t.dueAt = QDateTime::currentDateTimeUtc().addSecs(i * 3600);
        tasks.append(t);
    }
    const QVector<PriorityRecommendation> top = PriorityEngine::top3FromRules(
        PriorityEngine::applyRuleScores(tasks));
    QCOMPARE(top.size(), 3);
    for (const PriorityRecommendation &rec : top) {
        QVERIFY(rec.reason.size() >= 10);
    }
}

void TestCore::ut_S03_03_settingsRoundTrip()
{
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    PortableSettings::setStorageDirectoryForTests(tmp.path());
    PortableSettings::initialize();

    QSettings s = PortableSettings::open();
    s.clear();

    LlmConfig ollama = LlmProviderFactory::defaultConfig(LlmProviderType::Ollama);
    ollama.model = QStringLiteral("qwen3:4b");
    LlmProviderFactory::saveProfile(ollama);

    LlmConfig kimi = LlmProviderFactory::defaultConfig(LlmProviderType::Kimi);
    kimi.apiKey = QStringLiteral("test-key");
    kimi.model = QStringLiteral("moonshot-v1-8k");
    LlmProviderFactory::saveProfile(kimi);
    LlmProviderFactory::saveToSettings(kimi);

    const LlmConfig loadedOllama = LlmProviderFactory::loadProfile(LlmProviderType::Ollama, QStringLiteral("qwen3:4b"));
    QCOMPARE(loadedOllama.model, QStringLiteral("qwen3:4b"));

    const LlmConfig active = LlmProviderFactory::loadFromSettings();
    QCOMPARE(active.provider, LlmProviderType::Kimi);
    QCOMPARE(active.apiKey, QStringLiteral("test-key"));

    const QStringList ollamaModels = LlmProviderFactory::savedModels(LlmProviderType::Ollama);
    QVERIFY(ollamaModels.contains(QStringLiteral("qwen3:4b")));

    s.clear();
    PortableSettings::resetTestOverrides();
}

void TestCore::ut_S05_01_updateQuadrant()
{
    QTemporaryDir tmp;
    TaskRepository repo;
    QString err;
    QVERIFY(repo.open(tmp.path() + QStringLiteral("/t.db"), &err));

    TaskItem task;
    task.title = QStringLiteral("Move me");
    task.quadrant = EisenhowerQuadrant::Q4_NotUrgentNotImportant;
    QVERIFY(repo.addTask(&task, &err));

    QElapsedTimer timer;
    timer.start();
    QVERIFY(repo.updateTaskQuadrant(task.id, EisenhowerQuadrant::Q1_UrgentImportant, &err));
    QVERIFY(timer.elapsed() < 300);

    const TaskItem updated = repo.allTasks().first();
    QCOMPARE(updated.quadrant, EisenhowerQuadrant::Q1_UrgentImportant);
}

void TestCore::ut_S05_02_learningWeightsChange()
{
    QTemporaryDir tmp;
    TaskRepository repo;
    QString err;
    QVERIFY(repo.open(tmp.path() + QStringLiteral("/t.db"), &err));

    BehaviorLearningEngine engine(&repo);
    const ScoringWeights before = engine.currentWeights();

    for (int i = 0; i < 5; ++i)
        engine.recordEvent(BehaviorEventType::QuadrantChanged, i + 1, EisenhowerQuadrant::Q1_UrgentImportant);

    const ScoringWeights after = engine.currentWeights();
    QVERIFY(after.importance >= before.importance);
}

void TestCore::ut_S05_03_scoringUsesWeights()
{
    TaskItem task;
    task.quadrant = EisenhowerQuadrant::Q1_UrgentImportant;
    task.dueAt = QDateTime::currentDateTimeUtc().addSecs(3600);

    ScoringWeights low;
    low.urgency = 0.1;
    low.importance = 0.1;
    low.business = 0.1;
    low.effort = 0.7;
    low.normalize();

    ScoringWeights high;
    high.urgency = 0.4;
    high.importance = 0.4;
    high.business = 0.1;
    high.effort = 0.1;
    high.normalize();

    const double lowScore = PriorityEngine::computeRuleScore(task, low);
    const double highScore = PriorityEngine::computeRuleScore(task, high);
    QVERIFY(highScore > lowScore);
}

QTEST_APPLESS_MAIN(TestCore)
#include "tst_core.moc"
