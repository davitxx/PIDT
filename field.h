#ifndef FIELD_H_INCLUDED
#define FIELD_H_INCLUDED
#include "cell.h"
#include <list>


class Field
{
    public:
        Field();
        ~Field();
        void InitializeField(const char *file_n);
        void Moore();
        void vonNeumann();
        void PeoplePositions(const char *pfile_n);
        void Evacuation();
    private:
        struct Human_
        {
            int x;
            int y;
            bool isSafe;
            int id;
        };
        struct Human_out_
        {
            int new_x;
            int new_y;
            int old_x;
            int old_y;
            bool isSafe;
            int new_timestep_;
            int id;
        };
	struct Nei_
	{
	    int x;
	    int y;
	    double val;
	    int a;
	    bool moved;
	};
	Nei_ vals[8];
        Cell **cells_;
        Human_ *people_;
        std::list <Cell> list_cells_;
        std::list <Human_out_> list_out_;
        int s_x_, s_y_, numOfp_, numOfv_;
        double wall_, door_, f_val_, obstacle_, m_d_, m_s_;
        enum
        {
             cs_Unchecked = 0
            ,cs_Checked
            ,cs_Wall
            ,cs_Obstacle
            ,cs_Door
            ,cs_isOccupied
            ,cs_isNotOccupied
            ,cs_tempOccupied
        };
        void SetCell(int y, int x, double val);
        void DestroyCells();
        void DestroyPeople();
        void CreateEmptyField();
        void CreateDoor(int x, int y);
        void CreateObstacle(int x, int y);
        void SaveField();
        void SavePeoplePositions();
	void SaveValuePosition(int Y, int X, int a);
	void CheckMove(int na, int CX, int CY, int CXp, int CYp);
	void CheckMove(int na, int CX, int CY, int CXp, int CYp, int CXp1, int CYp1, int CXp2, int CYp2);
};
#endif // FIELD_H_INCLUDED