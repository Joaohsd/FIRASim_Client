#ifndef ROBOT_H
#define ROBOT_H


#include <QPoint>
#include <src/defines.h>
#include <src/uvf.h>
#include <src/staticdata.h>
#include <clients/actuator/actuatorclient.h>
#include <src/pid.h>
#include <iostream>
#include <QThread>

class Robot : public QThread
{
    Q_OBJECT
public slots:
    void printStartGOAL();
    void printStartDEF();
    void printStartATT();
    void printFinishGOAL();
    void printFinishDEF();
    void printFinishATT();
signals:
    void endPlay();
    public:
        Robot(int, int, double, double, double, double, double);
        void setRobot(int, int, double, double, double, double, double);
        void run();
        void playGoalKeeper();
        void playDefender();
        void playStricker();

        int getId();
        int getTeamId();

        double getX();
        double getY();
        double getVx();
        double getVy();
        double getVo();
        double getAngle();

        void setId(int);
        void setTeamId(int);
        void setX(double);
        void setY(double);
        void setVx(double);
        void setVy(double);
        void setVo(double);
        void setAngle(double);

        void setActuator(ActuatorClient* command);
        void setData(StaticData* data);

        void sendFIRA(int id, double wheelL, double wheelR);

        //Métodos de ação dos Robos
        void attack();  //OK

        void kick();    //OK

        void intercept();   //OK

        void go_to(QPointF target, double target_angle, bool have_obstacle, int decay_mode); //OK
        void go_to_kick(QPointF target, double speed_factor);   //OK
        void go_to_debug(); //OK
        void go_to_LARC(QPointF target, double target_angle, bool have_obstacle);
        void go_to_penalty();
        void go_to_defend_penalty();

        void spin();            //OK
        void spin_x();          //OK
        void spin_ataque();     //OK
        void spin_goleiro();    //OK
        void spin_penalty();    //OK

        void position(QPointF point, double angle, bool has_obstacle, int speed_mode);

        void goleiro_semUVF();
        void defend_goal();
        void defend_block(int);
        void defend_middle();           //OK
        void defend_middle_attack();    //OK
        void defend_goal_new();
        void defend_goal_middle();
        void defend_center();           //OK
        void defend_goal_bao();

        void defend_goal_LARC_2();

        void defend_goal_LARC();
        void defend_goal_Left();
        void defend_goal_Right();

        void defend_attack();

        void position_atacantes(QPointF point, double angle, bool has_obstacle, int speed_mode);

        void set_angle(double heading_angle);   //OK
        void set_angle(QPointF heading_point);  //OK

        void penalty();

        void stopRobot();

        void containment();

        UVF path;

        QPointF pos, fut_pos;

        /**
         * @brief get_angle
         * @param vec_a Vector A
         * @param vec_b Vector B
         * @return Returns the angle between two vectors
         */

        inline double get_angle(QPointF vec_a, QPointF vec_b){
            double ans = atan2(vec_b.y(),vec_b.x()) - atan2(vec_a.y(),vec_a.x());
            if(ans < -PI)
                ans += 2*PI;
            else if(ans > PI)
                ans -= 2*PI;
            return ans;
        }

    private:

        int player_id;
        int team_id;
        double x;
        double y;
        double vx;
        double vy;
        double vo;
        double angle;

        bool kick_stat = false;

        double speed_kick_base = 0.8;
        double speed_kick_increment = 0.3;
        double speed_kick_max = 1.4;
        double speed_kick_current = 0.0;

        double base_speed = 0.0, spin_speed = 0.0;

        bool defend_spin = false;

        double KP = 0.0, KI = 0.0, KD = 0.0, input = 0.0, setpoint = 0.0, phi = 0.0, angulo = 0.0, output = 0.0, curr_speed = 0.0;

        PID path_pid = PID(&input, &setpoint);
        PID angle_pid = PID(&input, &setpoint);


        ActuatorClient* actuator;

        pair<double,double> speed = make_pair(0.0,0.0);

        StaticData *data;


        /**
         * @brief distance
         * @param point_a Point A
         * @param point_b Point B
         * @return Returns the euclidian distance between two points
         */
        inline double distance(QPointF point_a, QPointF point_b){
            double xx = point_a.x() - point_b.x();
            double yy = point_a.y() - point_b.y();
            return sqrt(xx*xx + yy*yy);
        }

        /**
         * @brief medium_qpoint
         * @param point_a Point A
         * @param point_b Point B
         * @return Returns the medium point betwen point A and B
         */
        inline QPointF medium_qpoint(QPointF point_a, QPointF point_b){
            return QPointF((point_a.x() + point_b.x())/2,(point_a.y() + point_b.y())/2);
        }

        /**
         * @brief border_y
         * @return Verify if the ball is in the y border of the field
         */
        inline bool border_y(){
            double min_dist = 0.1;
            double y = data->ballPos.y();
            if(y >= data->max_field.y() - min_dist || y <= data->min_field.y() + min_dist)
                return 1;
            else
                return 0;
        }

        /**
         * @brief border_x
         * @return Verify if the ball is in the x border of the field
         */
        inline bool border_x(){
            double min_dist = 0.1;
            double x = data->ballPos.x();
            if(x >= data->max_field.x() - min_dist || x <= data->min_field.x() + min_dist)
                return 1;
            else
                return 0;
        }

        /**
         * @brief line_intersect
         * @param value_a Point A
         * @param value_b Point B
         * @param value_c Point C
         * @param value_d Point D
         * @return Returns the intersection between points A, B and C,D
         */
        inline QPointF line_intersect(QPointF value_a, QPointF value_b, QPointF value_c, QPointF value_d){
            double x1 = value_a.x(), x2 = value_b.x(), x3 = value_c.x(), x4 = value_d.x();
            double y1 = value_a.y(), y2 = value_b.y(), y3 = value_c.y(), y4 = value_d.y();
            double p_x = (x1*y2 - y1*x2)*(x3 - x4) - (x1 - x2)*(x3*y4 - y3*x4);
            double p_y = (x1*y2 - y1*x2)*(y3 - y4) - (y1 - y2)*(x3*y4 - y3*x4);
            double div = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);
            if(div == 0)
                div = 0.001;
            return QPointF(p_x/div,p_y/div);
        }

        /**
         * @brief is_inside
         * @param point Point A
         * @param field Field A
         * @return Returns true if the point a is inside the field A, and false
         * otherwise
         */
        inline bool is_inside(QPointF point, FieldSpace field){
            double x = point.x();
            double y = point.y();
            if(x <= field.getBottomRight().x() && x >= field.getBottomLeft().x())
            if(x <= field.getTopRight().x() && x >= field.getTopLeft().x())
            if(y <= field.getTopRight().y() && y >= field.getBottomRight().y())
            if(y <= field.getTopLeft().y() && y >= field.getBottomLeft().y())
                return 1;
            return 0;
        }

        inline bool verify_Strickers_Left_Is_Inside_Attack_Area(){
             if(is_inside(this->data->ballPos,this->data->area[RIGHT_SIDE]) && (is_inside(this->data->player[TEAM_BLUE][ATT_ID],this->data->area[RIGHT_SIDE]) || is_inside(this->data->player[TEAM_BLUE][DEF_ID],this->data->area[RIGHT_SIDE]) || this->data->player[TEAM_BLUE][ATT_ID].x() > this->data->max_field.x() || this->data->player[TEAM_BLUE][DEF_ID].x() > this->data->max_field.x()))
                return true;
             else return false;
        }

        inline bool verify_Strickers_Right_Is_Inside_Attack_Area(){
             if(is_inside(this->data->ballPos,this->data->area[LEFT_SIDE]) && (is_inside(this->data->player[TEAM_YELLOW][ATT_ID],this->data->area[LEFT_SIDE]) || is_inside(this->data->player[TEAM_YELLOW][DEF_ID],this->data->area[LEFT_SIDE]) || this->data->player[TEAM_YELLOW][ATT_ID].x() < this->data->min_field.x() || this->data->player[TEAM_YELLOW][DEF_ID].x() < this->data->min_field.x()))
                return true;
             else return false;
        }
};
#endif // ROBOT_H

