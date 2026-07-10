#include "line_number_plain_text_edit.h"

#include "../utils/app_theme.h"

#include <QAbstractTextDocumentLayout>
#include <QFontMetrics>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTextBlock>

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(LineNumberPlainTextEdit *editor)
        : QWidget(editor)
        , m_editor(editor)
    {
    }

    QSize sizeHint() const override
    {
        return {m_editor->lineNumberAreaWidth(), 0};
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        m_editor->paintLineNumberArea(event);
    }

private:
    LineNumberPlainTextEdit *m_editor = nullptr;
};

LineNumberPlainTextEdit::LineNumberPlainTextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_lineNumberArea = new LineNumberArea(this);

    connect(this, &LineNumberPlainTextEdit::blockCountChanged,
            this, &LineNumberPlainTextEdit::updateLineNumberAreaWidth);
    connect(this, &LineNumberPlainTextEdit::updateRequest,
            this, &LineNumberPlainTextEdit::updateLineNumberArea);

    updateLineNumberAreaWidth(0);

    QFont f = font();
    f.setPixelSize(AppTheme::metrics().fontBase);
    setFont(f);

    QTextDocument *doc = document();
    doc->setDocumentMargin(4);
    QTextOption opt = doc->defaultTextOption();
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    doc->setDefaultTextOption(opt);
}

void LineNumberPlainTextEdit::setLineNumberAreaVisible(bool visible)
{
    if (m_lineNumbersVisible == visible)
        return;
    m_lineNumbersVisible = visible;
    m_lineNumberArea->setVisible(visible);
    updateLineNumberAreaWidth(blockCount());
}

bool LineNumberPlainTextEdit::lineNumberAreaVisible() const
{
    return m_lineNumbersVisible;
}

void LineNumberPlainTextEdit::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(
        QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

int LineNumberPlainTextEdit::lineNumberAreaWidth() const
{
    if (!m_lineNumbersVisible)
        return 0;

    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    const int padding = 8;
    return padding + fontMetrics().horizontalAdvance(QStringLiteral("9")) * digits + padding;
}

void LineNumberPlainTextEdit::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void LineNumberPlainTextEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void LineNumberPlainTextEdit::paintLineNumberArea(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(0x08, 0x10, 0x1e));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    const int currentBlock = textCursor().blockNumber();
    const QColor numberColor(0x6a, 0x7a, 0x98);
    const QColor currentColor(0x9a, 0xae, 0xcc);

    painter.setFont(font());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString number = QString::number(blockNumber + 1);
            painter.setPen(blockNumber == currentBlock ? currentColor : numberColor);
            painter.drawText(0, top, m_lineNumberArea->width() - 6, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        ++blockNumber;
        bottom = top + qRound(blockBoundingRect(block).height());
        top = bottom;
    }
}
