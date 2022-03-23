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
    //Actions
    void printStartGOAL();
    void printStartDEF();
    void printStartATT();
    void printFinishGOAL();
    void printFinishDEF();
    void printFinishATT();

    public:
        Robot(int, int, double, double, double, double, double);
        void run();

        //Métodos de ação dos Robos

        //Gerais
        void intercept();
        void intercept_antigo();

        void go_to(QPointF target, double target_angle, bool have_obstacle, int decay_mode);
        void go_to_debug();

        void spin();
        void spin_x();
        void spin_ataque();
        void spin_goleiro();
        void spin_penalty();

        void position(QPoint point, double angle, bool has_obstacle, int speed_mode);

        void set_angle(double heading_angle);
        void set_angle(QPoint heading_point);

        void defend_middle();
        void defend_middle_attack();
        void defend_center();
        void defend_block(int);

        void containing();

        void stopRobot();

        //Stricker
        void attack();
        void attack_antigo();
        void kick();
        void go_to_kick(QPoint target, double speed_factor);
        void go_to_penalty();
        void defend_attack();

        //Defender
        void defend();

        //Goalkeeper
        void defend_goal_libero();
        void defend_goal_safe();
        void defend_goalLine_Left();
        void defend_goalLine_Right();
        void intercept_goalkeeper(bool hasObstacle);

        void playID_2();
        void playID_1();
        void playID_0();

        //GETTERS
        int getId();
        int getTeamId();
        int getX();
        int getY();
        int getVx();
        int getVy();
        double getVo();
        double getAngle();
        //SETTERS
        void setId(int);
        void setTeamId(int);
        void setX(int);
        void setY(int);
        void setVx(int);
        void setVy(int);
        void setVo(double);
        void setAngle(double);

        void setActuator(ActuatorClient* command);
        void sendFIRA(int id, double wheelL, double wheelR);

        UVF path;

        QPoint pos, fut_pos;

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
        int x;
        int y;
        int vx;
        int vy;
        double vo;
        double angle;

        bool kick_stat = false;
        bool pos_stat = false;
        bool leaveFromGoal = false;

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
        inline QPoint medium_qpoint(QPoint point_a, QPoint point_b){
            return QPoint((point_a.x() + point_b.x())/2,(point_a.y() + point_b.y())/2);
        }

        /**
         * @brief border_y
         * @return Verify if the ball is in the y border of the field
         */
        inline bool border_y(){
            double min_dist =100;
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
            double min_dist = 100;
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
        inline QPoint line_intersect(QPoint value_a, QPoint value_b, QPoint value_c, QPoint value_d){
            int x1 = value_a.x(), x2 = value_b.x(), x3 = value_c.x(), x4 = value_d.x();
            int y1 = value_a.y(), y2 = value_b.y(), y3 = value_c.y(), y4 = value_d.y();
            int p_x = (x1*y2 - y1*x2)*(x3 - x4) - (x1 - x2)*(x3*y4 - y3*x4);
            int p_y = (x1*y2 - y1*x2)*(y3 - y4) - (y1 - y2)*(x3*y4 - y3*x4);
            double div = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);
            if(div == 0)
                div = 0.001;
            return QPoint(p_x/div,p_y/div);
        }

        /**
         * @brief is_inside
         * @param point Point A
         * @param field Field A
         * @return Returns true if the point a is inside the field A, and false
         * otherwise
         */
        inline bool is_inside(QPoint point, FieldSpace field){
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
             if(is_inside(this->data->ballPos,this->data->area[RIGHT_SIDE]) && (is_inside(this->data->player[TEAM_BLUE][ID_2],this->data->area[RIGHT_SIDE]) || is_inside(this->data->player[TEAM_BLUE][ID_1],this->data->area[RIGHT_SIDE]) || this->data->player[TEAM_BLUE][ID_2].x() > this->data->max_field.x() || this->data->player[TEAM_BLUE][ID_1].x() > this->data->max_field.x()))
                return true;
             else return false;
        }

        inline bool verify_Strickers_Right_Is_Inside_Attack_Area(){
             if(is_inside(this->data->ballPos,this->data->area[LEFT_SIDE]) && (is_inside(this->data->player[TEAM_YELLOW][ID_2],this->data->area[LEFT_SIDE]) || is_inside(this->data->player[TEAM_YELLOW][ID_1],this->data->area[LEFT_SIDE]) || this->data->player[TEAM_YELLOW][ID_2].x() < this->data->min_field.x() || this->data->player[TEAM_YELLOW][ID_1].x() < this->data->min_field.x()))
                return true;
             else return false;
        }

        inline int verify_Player_Best_Condition(int firstID, int secondID){
            double ball_player_angle_firstID = fabs(get_angle(QPoint(120*cos(data->player_angle[data->playTeam][firstID]),120*sin(data->player_angle[data->playTeam][firstID])),QPointF(data->futureBallPos - pos)))*180/PI;
            double ball_player_angle_secondID = fabs(get_angle(QPoint(120*cos(data->player_angle[data->playTeam][secondID]),120*sin(data->player_angle[data->playTeam][secondID])),QPointF(data->futureBallPos - pos)))*180/PI;
            int distance_firstID_ball = distance(data->player[data->playTeam][firstID], data->futureBallPos);
            int distance_secondID_ball = distance(data->player[data->playTeam][secondID], data->futureBallPos);
            if(ball_player_angle_firstID >= 90.0){
                ball_player_angle_firstID = 180.0 - ball_player_angle_firstID;
            }
            if(ball_player_angle_secondID >= 90.0){
                ball_player_angle_secondID = 180.0 - ball_player_angle_secondID;
            }
            if(distance_secondID_ball <= distance_firstID_ball && ball_player_angle_secondID <= ball_player_angle_firstID){
                return secondID;
            }
            else if(distance_firstID_ball < distance_secondID_ball){
                return firstID;
            }
            else{
                return secondID;
            }

        }
};
#endif // ROBOT_H

