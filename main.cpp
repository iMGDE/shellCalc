#include <QApplication>
#include <QPair>
#include <QVector>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QStringBuilder>
#include <QComboBox>
#include <QScreen>
#include <QRect>
#include <QTime>
#include <QLineEdit>

#include "SSL.h"

void delay(int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

class SSLCalc : public QMainWindow
{
public:
    SSLCalc(QWidget *parent = nullptr): QMainWindow(parent)
    {
        //window Setup
        setWindowFlags(Qt::WindowStaysOnTopHint);
        resize(200,200);
        //buttonCalc
        buttonShoot->resize(180,20);
        buttonShoot->move(10,5);
        connect(buttonShoot, &QPushButton::pressed, this, &SSLCalc::buttonShootPushed);
        //buttonFind
        buttonReset->resize(180,20);
        buttonReset->move(10,30);
        connect(buttonReset, &QPushButton::pressed, this, &SSLCalc::buttonResetPushed);
        //shortcutFindMe & en
        connect(shortcutFindMe, &QShortcut::activated, this, &SSLCalc::buttonFindMe);
        connect(shortcutFindEn, &QShortcut::activated, this, &SSLCalc::buttonFindEn);

        //COORDS
        labelCoords->move(0,44);
        labelCoords->resize(100,50);
        labelCoords->setAlignment(Qt::AlignRight);
        labelCoords->setText("Own Coords:\nEnemy Coords:");
        labelCoordsMe->move(105,49);
        labelCoordsMe->resize(150,20);
        labelCoordsMe->setText("Ctrl+Alt+R");
        labelCoordsEn->move(105,69);
        labelCoordsEn->resize(150,20);
        labelCoordsEn->setText("Ctrl+Alt+E");

        //Wind
        labelWind->move(10,92);
        labelWind->resize(40,20);
        labelWind->setAlignment(Qt::AlignRight);
        labelWind->setText("Wind: ");
        textWind->move(50,90);
        textWind->resize(140,20);
        textWind->setText("0");

        //comboShot
        comboShot->move(10,120);
        comboShot->resize(180,30);
    }
    ~SSLCalc(){}
protected:
    void buttonShootPushed()
    {
        /*if(comboShot->currentText().isEmpty()) return;
        //move mouse to tank
        delay(500);
        SSL::Input::moveMouse(myPos.x(),myPos.y()-10);
        //lmt down
        SSL::Input::tabInGame();
        delay(100);
        SSL::Input::click();
        delay(20);
        SSL::Input::click();
        delay(100);
        //move way above tank
        SSL::Game::moveGunPow(100);
        //press power//whatever key to do shit
        int power = comboShot->currentText().split(", ").first().toInt();
        int angle = comboShot->currentText().split(", ").last().toInt();
        qDebug("%d power ;; %d angle", power, angle);
        for(int i=0; i<90-angle;i++){
            if(myPos.x() > enemyPos.x()){
                SSL::Input::moveGunLeft();
            } else {
                SSL::Input::moveGunRight();
            }
            delay(30);
        }
        for(int i=100;i!=power;i--){
            SSL::Input::movePowerDown();
            delay(30);
        }
        //press space to shoot
        SSL::Input::shoot();*/

    }

    void CalcAllAndAdd()
    {
        if(myPos.isNull() || enemyPos.isNull()) return;
        comboShot->clear();
        for(auto x : SSL::Calc::calcAllPossibleAngles(myPos,enemyPos, textWind->text().toInt()))
        {
            comboShot->addItem(QString::number(x.pow) % QString(", ") % QString::number(x.ang));
        }

    }
    void buttonResetPushed()
    {
        buttonFindMe(); buttonFindEn();
    }
    void buttonFindMe()
    {
        QRect sc = QGuiApplication::primaryScreen()->geometry();
        QPoint tmp, m = QCursor::pos();
        tmp.setX(m.x());
        tmp.setY(sc.height()-m.y());
        myPos = tmp;
        labelCoordsMe->setText(QString::number(myPos.x()) % QString(", ") %QString::number(myPos.y()));
        if(myPos.x() != enemyPos.x()){
            CalcAllAndAdd();
        }
    }
    void buttonFindEn()
    {
        QRect sc = QGuiApplication::primaryScreen()->geometry();
        QPoint tmp, m = QCursor::pos();
        tmp.setX(m.x());
        tmp.setY(sc.height()-m.y());
        enemyPos = tmp;
        labelCoordsEn->setText(QString::number(enemyPos.x()) % QString(", ") %QString::number(enemyPos.y()));
        if(myPos.x() != enemyPos.x()){
            CalcAllAndAdd();
        }
    }

private:
    QLabel  *labelCoordsMe = new QLabel(this),
            *labelCoordsEn = new QLabel(this),
            *labelCoords = new QLabel(this),
            *labelWind = new QLabel(this);
    QComboBox   *comboShot = new QComboBox(this);
    QPushButton *buttonShoot = new QPushButton("Shoot" ,this),
                *buttonReset = new QPushButton("Reset Positions", this);
    QLineEdit   *textWind = new QLineEdit(this);
    QShortcut   *shortcutFindMe = new QShortcut(tr("Ctrl+Alt+R"),this),
                *shortcutFindEn = new QShortcut(tr("Ctrl+Alt+E"),this);
    QPoint myPos, enemyPos;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SSLCalc w;
    w.show();
    return a.exec();
}
