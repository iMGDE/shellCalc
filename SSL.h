#ifndef SSL_H
#define SSL_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <map>
#include <QString>
#include <QStringBuilder>
#include <QPoint>
#include <QPair>
#include <QVector>

struct Gun{
    int pow, ang;
};

namespace SSL {
    namespace {
        std::string exec(const char* cmd) {
            std::array<char, 128> buffer;
            std::string result;
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }
            return result;
        }
        QString x11_id = QString(exec("xdotool search --name \"ShellShock Live\"").c_str()).replace(QString("\n"),QString(""));
        QString base_key = QString(QString("xdotool key --window ") % x11_id % QString(" "));
        double rad(double x){
            //qDebug("%f° = rad %f", x, x*M_PI/180);
            return x*M_PI / 180;
        }
    }
    //calc commands
    namespace Calc {
        const double g = -379.106, q = 0.0518718, a = 0.4757;
        double calcPower(int distanceX, int distanceY, int angle){
            //v0 = -2/(g * q) * math.sqrt((-g * distancex * distancex)/(2 * math.cos(math.radians(angle)) * math.cos(math.radians(angle)) * (math.tan(math.radians(angle)) * distancex - distancey)))
            return -2/(g * q) * sqrt((-g * (distanceX * distanceX))/(2 * (cos(rad(angle))*cos(rad(angle))) * (tan(rad(angle)) * distanceX - distanceY)));
        }
        std::map<int,int> calcTrajectoryWind(double x, double y, int angle, int power, int wind, double timestep = 0.1, double maxTime = 10){
                double v = (-g*q)/2 * power;
                double v_x = v * cos(rad(angle));
                double v_y = v * sin(rad(angle));

                std::map<int,int> trajectory;

                for(double time=0; time<maxTime; time+=timestep){
                    double w = wind * a;
                    double deltaX = v_x * timestep + w/2 * timestep * timestep;
                    double deltaY = v_y * timestep + g/2 * timestep * timestep;
                    double deltaVX = w * timestep;
                    double deltaVY = g * timestep;
                    trajectory.insert({x,1080-y});
                    x = x + deltaX;
                    y = y + deltaY;
                    v_x = v_x + deltaVX;
                    v_y = v_y + deltaVY;
                    if(1080-y < 0 or x < -200 or x > 2200)
                        break;
                }
                return trajectory;
        }
        bool checkHit(QPoint hit, std::map<int,int> traj, double maxDistance = 10){
            for (int i = traj.begin()->first; i<traj.end()->first; i++) {
                double d = distancePointFromLine(hit.x(), hit.y(), i, traj.at(i), i+1, traj.at(i+1));
                if (d <= maxDistance)
                    return 1;
            }
            return 0;
        }


        QVector<Gun> calcAllPossibleAngles(QPoint myPos, QPoint enemyPos, int wind = 0){
            QVector<Gun> shots;
            int distanceX = enemyPos.x() - myPos.x();
            int distanceY = enemyPos.y() - myPos.y();

            //if(distanceX < 0) {
            //    distanceX = 0-distanceX;
            //}

            qDebug("distX=%d ;; distY=%d", distanceX,distanceY);
            double power;
            for(int angle = -89; angle<90;angle++) {
                if(wind == 0){
                    power = calcPower(distanceX,distanceY,angle);
                    qDebug("power %f, angle %d ", power, angle);
                } else {
                    power = calcPowerWind(distanceX,distanceY,angle, wind);
                }
                if(0 < power && power < 101) {
                    shots.append(Gun{int(std::round(power)),angle} );
                }
            }
            return shots;
        }
    };

    namespace Input {
        void moveMouse(int x, int y){
            system(QString(QString("xdotool mousemove ") % QString::number(x) % QString(" ") % QString::number(y)).toStdString().c_str());
        }
        void tabInGame(){
            system(QString(QString("xdotool windowactivate ") % x11_id).toStdString().c_str());
        }
        void click(int key = 1){
            system(QString(QString("xdotool click ") % QString::number(key)).toStdString().c_str());
        }
    }

    namespace Game {
        //TODO: find out how, cheat or opencv
        QVector<QPoint> getAllEnemyTankPos();
        QVector<QPoint> getMyTankPos();
        Gun getMyGun();

        void moveTank(int x);
        void setGun(Gun gun);
        void moveGunAng(int delta);
        void moveGunPow(int delta);
        void shootGun();
    }
}

#endif // SSL_H
