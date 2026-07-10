#ifndef LINE_NUMBER_PLAIN_TEXT_EDIT_H
#define LINE_NUMBER_PLAIN_TEXT_EDIT_H

#include <QPlainTextEdit>

class LineNumberArea;

class LineNumberPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
    friend class LineNumberArea;
public:
    explicit LineNumberPlainTextEdit(QWidget *parent = nullptr);

    void setLineNumberAreaVisible(bool visible);
    bool lineNumberAreaVisible() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    void paintLineNumberArea(QPaintEvent *event);
    int lineNumberAreaWidth() const;

    LineNumberArea *m_lineNumberArea = nullptr;
    bool m_lineNumbersVisible = true;
};

#endif // LINE_NUMBER_PLAIN_TEXT_EDIT_H
