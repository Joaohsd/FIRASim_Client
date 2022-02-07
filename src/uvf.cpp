#include "uvf.h"

UVF::UVF(QObject *parent) : QObject(parent)
{

}

vector<QPoint> UVF::get_obstacles(int team_id, int player_id, bool has_obstacles)
{
    vector<QPoint> obstacles;
    if(!has_obstacles)
        return obstacles;
    for(int i = 0; i < 2; ++i)
        for(int j = 1; j < 4; ++j)
                        if(!(i == team_id && j == player_id))
                obstacles.push_back(data->player[i][j]);
    return obstacles;
}

double UVF::get_phi(QPoint origin, QPointF target, double target_ori, vector<QPoint> obstacles)
{
    double phi = 0.0;
    target_ori = constrain_angle(target_ori);
    double rot = PI - target_ori;
    QPointF temp_origin = origin;
    origin.setX(temp_origin.x()*cos(rot) - temp_origin.y()*sin(rot));
    origin.setY(temp_origin.x()*sin(rot) + temp_origin.y()*cos(rot));
    QPointF temp_target = target;
    target.setX(temp_target.x()*cos(rot) - temp_target.y()*sin(rot));
    target.setY(temp_target.x()*sin(rot) + temp_target.y()*cos(rot));
    double x = origin.x() - target.x();
    double y = origin.y() - target.y();
    double phi_tuf = 0;
    double yL = y + data->k_de;
    double yR = y - data->k_de;
    QPoint pL(x,yL);
    QPoint pR(x,yR);
    if(y < -data->k_de)
        phi_tuf = get_phih(pL,0,0,1);
    else if(y >= data->k_de)
        phi_tuf = get_phih(pR,0,0,0);
    else{
        double phi_hccw = get_phih(pR,0,0,0);
        double phi_hcw = get_phih(pL,0,0,1);
        QPointF nh_cw(cos(phi_hcw),sin(phi_hcw));
        QPointF nh_ccw(cos(phi_hccw),sin(phi_hccw));
        double phi_px = (fabs(yL)*nh_ccw.x() + fabs(yR)*nh_cw.x())/(2.0*data->k_de);
        double phi_py = (fabs(yL)*nh_ccw.y() + fabs(yR)*nh_cw.y())/(2.0*data->k_de);
        phi_tuf = atan2(phi_py,phi_px);
    }
    for(int i = 0; i < static_cast<int>(obstacles.size()); ++i){
        QPointF obstacle = obstacles[i];
        QPointF temp_obstacle = obstacle;
        obstacle.setX(temp_obstacle.x()*cos(rot) - temp_obstacle.y()*sin(rot));
        obstacle.setY(temp_obstacle.x()*sin(rot) + temp_obstacle.y()*cos(rot));
        QPointF auf;
        auf.setX(origin.x() - obstacle.x());
        auf.setY(origin.y() - obstacle.y());
        double phi_auf = atan2(auf.y(),auf.x());
        phi_tuf = constrain_angle(phi_tuf);
        phi_auf = constrain_angle(phi_auf);
        double r = sqrt(auf.x()*auf.x() + auf.y()*auf.y());
        if(r <= data->k_dmin)
            phi = phi_auf;
        else{
            double gauss = get_gauss(r - data->k_dmin, data->k_delta);
            double diff = fabs(phi_auf - phi_tuf);
            if(diff > PI)
                diff = fabs(2*PI - diff);
            double cross = cos(phi_auf)*sin(phi_tuf) - sin(phi_auf)*cos(phi_tuf);
            double s;
            if(cross > 0)
                s = -1.0;
            else
                s = 1.0;
            phi = phi_tuf + s*diff*gauss;
            QPointF vec_obs(obstacle.x() - target.x(), obstacle.y() - target.y());
            if(auf.x() < 0)
            if(!(vec_obs.y() > 0 && vec_obs.x() > 0 && auf.y() < 0 && auf.x() < 0))
            if(!(vec_obs.y() < 0 && vec_obs.x() > 0 && auf.y() > 0 && auf.x() < 0))
            if(!(vec_obs.y() > 0 && auf.y() >= 0 && auf.x() < 0))
            if(!(vec_obs.y() < 0 && auf.y() <= 0 && auf.x() < 0))
                phi = phi_tuf + (phi_auf - phi_tuf)*gauss;
        }
        phi_tuf = phi;
    }
    phi = phi_tuf;
    phi = phi - rot;
    return phi;
}

double UVF::get_phih(QPoint p, double tx, double ty, double ccw)
{
    double signal = 1;
    double phih;
    if(ccw == 1)
        signal = -1;
    double theta = atan2(p.y() - ty, p.x() - tx);
    double ro = sqrt((p.x() - tx)*(p.x() - tx) + (p.y() - ty)*(p.y() - ty));
    if(ro > data->k_de)
        phih = theta + signal*(PI/2)*(2 -((data->k_de + data->k_kr)/(ro + data->k_kr)));
    else
        phih = theta + signal*(PI/2)*sqrt(ro/data->k_de);
    return phih;
}

double UVF::get_angle(QPoint origin, QPointF target, double target_ori, int team_id, int player_id, bool has_obs)
{
    vector<QPoint> obs = get_obstacles(team_id, player_id, has_obs);
    double phi = get_phi(origin,target,target_ori,obs);
    return phi;
}
