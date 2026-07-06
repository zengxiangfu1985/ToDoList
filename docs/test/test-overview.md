# 测试总览

## ID 规范

- UT-Sxx-nn：单元测试
- ST-Sxx-nn：集成/信号测试

## AC 追溯

| AC | 用例 ID |
|----|---------|
| AC-S01-1 | UT-S01-01, ST-S01-01 |
| AC-S01-EX1 | UT-S01-02 |
| AC-S02-1 | UT-S02-01 |
| AC-S02-EX1 | UT-S02-02 |
| AC-S03-1 | UT-S03-01 |
| AC-S03-EX1 | UT-S03-02 |

## Fixtures

- 临时 SQLite `:memory:` 或 QTemporaryDir
- Mock LlmProvider 用于无网络测试
