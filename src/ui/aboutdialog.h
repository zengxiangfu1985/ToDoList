#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class QShowEvent;

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog() override;

signals:
    void checkUpdateRequested();
    void importOfflineUpdateRequested(const QString &zipPath);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onCheckUpdate();
    void onImportOfflineUpdate();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
