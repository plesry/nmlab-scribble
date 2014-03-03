#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>

#include "common.h"
#include "scribblearea.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void open();
    void save();
    void undo();
    void redo();
    void penColor();
    void brushColor();
    void penWidth();
    void lineWidth();
    void about();
    void shape(QAction *);
    void line();
    void brush();

private:
    void createSaveAsMenu();

    void createActionGroups();
    void setData();
    void connectAct();
    void setActShortcuts();

    bool maybeSave();
    bool saveFile(const QByteArray &fileFormat);

    Ui::MainWindow *ui;
    ScribbleArea *scribbleArea;

    QList<QAction *> saveAsActs;
    QActionGroup *drawActionGroup;
};

#endif
