#ifndef CELL_H_INCLUDED
#define CELL_H_INCLUDED

class Cell
{
    public:
        inline int GetState() {return state_;};
        inline int GetOccState() {return occ_;};
        inline double GetValue() {return value_;}
        inline int GetX() {return pos_x_;}
        inline int GetY() {return pos_y_;}
        void SetValue(double v) {value_ = v;}
        void SetState(int s) {state_ = s;}
        void SetOccState(int s) {occ_ = s;}
        void SetX(int x) {pos_x_ = x;}
        void SetY(int y) {pos_y_ = y;}

    private:
        int state_;          // Stan komorki
        int occ_;            // Stan zajetosci komorki
        int pos_x_, pos_y_;  // Pozycja komorki w tablicy
        double value_;       // Wartosc komorki
};

#endif // CELL_H_INCLUDED
