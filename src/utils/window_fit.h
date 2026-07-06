#ifndef WINDOW_FIT_H
#define WINDOW_FIT_H

class QDialog;
class QMainWindow;
class QRect;
class QWidget;

namespace WindowFit {

QRect availableGeometry(const QWidget *widget);
void fitMainWindow(QMainWindow *window);
void fitDialog(QDialog *dialog, int preferredWidth = 520);

} // namespace

#endif // WINDOW_FIT_H
