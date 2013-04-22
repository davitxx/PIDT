#include "field.h"
#include <fstream>
#include <iostream>

Field::Field()
{
    door_ = 1.0;        // Wartosc komorki opisujacej drzwi
    wall_ = 600.0;      // Wartosc komorki opisujacej sciane/przeszkode
    obstacle_ = 500.0;  // Wartosc komorki opisujacej przeszkode
    f_val_ = 900.0;     // Warotsc komorki przed wypelnianiem pola
    m_d_ = 1.5;         // Ruch po skosie
    m_s_ = 1.0;         // Ruch po prostej
    numOfv_ = 0;
    
}

Field::~Field()
{
    DestroyPeople();
    DestroyCells();
}

void Field::DestroyCells()
{
    for (int a=0; a<s_y_; a++) delete [] cells_[a];
    delete [] cells_;
}

void Field::DestroyPeople()
{
    delete [] people_;
}

void Field::InitializeField(const char *file_n)
{
    std::fstream in;
    in.open(file_n, std::fstream::in);
    int n;  // Liczba przedmiotow
    int cn;  // Liczba komorek dla jednego
    int x,y;  // Wspolrzedne komorki

    // Tworzenie pomieszczenia
    in >> s_x_;
    in >> s_y_;

    cells_ = new Cell*[s_y_];
    for (int a=0; a<s_y_; a++)
    {
        cells_[a] = new Cell[s_x_];
    }

    CreateEmptyField();

    // Dodawanie drzwi
    in >> n; // Pobieranie calkowitej liczby drzwi
    in >> cn; // Pobieranie ilosci komorek dla pierwszych drzwi
    for (int a=0; a<n; a++)
    {
        for (int b=0; b<cn; b++)
        {
            in >> x;
            in >> y;
            CreateDoor(x,y);
        }
        if (a < n-1) in >> cn;
    }

    // Dodawanie przeszkod
    in >> n; // Pobieranie calkowitej liczby przeszkod
    in >> cn; // Pobieranie ilosci komorek dla pierwszej przeszkody
    for (int a=0; a<n; a++)
    {
        for (int b=0; b<cn; b++)
        {
            in >> x;
            in >> y;
            CreateObstacle(x,y);
        }
        if (a < n-1) in >> cn;
    }
    in.close();
}

void Field::PeoplePositions(const char *pfile_n)
{
    std::fstream in;
    in.open(pfile_n, std::fstream::in);
    int n=0;  // Liczba osob
    int x,y;  // Wspolrzedne komorki
    int state = 0; // Stan aktualnie sprawdzanej komorki
    in >> n;
    numOfp_ = n;
    people_ = new Human_[n];

    for (int a=0; a<n; a++)
    {
        in >> x;
        in >> y;
        state = cells_[y-1][x-1].GetState();
        if (state != cs_Wall && state != cs_Obstacle && state != cs_Door && state != cs_isOccupied)
        {
            cells_[y-1][x-1].SetOccState(cs_isOccupied);
            people_[a].y = y-1;
            people_[a].x = x-1;
            people_[a].isSafe = false;
            people_[a].id = a;
        }
    }
    in.close();
    //Sortowanie tablicy
    Human_ tmp;
    double valB, valA;
    for(int b=0; b<n; b++)
        for(int a=b; a<n; a++)
        {
            valB = cells_[people_[b].y][people_[b].x].GetValue();
            valA = cells_[people_[a].y][people_[a].x].GetValue();
            if (valB > valA)
            {
                tmp = people_[a];
                people_[a] = people_[b];
                people_[b] = tmp;
            }
        }
    SaveField();
}

void Field::CreateEmptyField()
{
    int a=0, b=0;
    // Wypelnianie poczatkowymi wartosciami
    for(a=0; a<s_y_; a++)
        for(b=0; b<s_x_; b++)
        {
            cells_[a][b].SetValue(f_val_);
            cells_[a][b].SetY(a);
            cells_[a][b].SetX(b);
            cells_[a][b].SetState(cs_Unchecked);
        }
    // Dodawanie scian
    for(a=0; a<s_x_; a++)
    {
        cells_[0][a].SetValue(wall_);
        cells_[0][a].SetState(cs_Wall);
        cells_[s_y_-1][a].SetValue(wall_);
        cells_[s_y_-1][a].SetState(cs_Wall);
    }
    for(a=1; a<s_y_-1; a++)
    {
        cells_[a][0].SetValue(wall_);
        cells_[a][0].SetState(cs_Wall);
        cells_[a][s_x_-1].SetValue(wall_);
        cells_[a][s_x_-1].SetState(cs_Wall);
    }
}

void Field::CreateDoor(int x,int y)
{
    if (cells_[y-1][x-1].GetState() == cs_Wall)
    {
        cells_[y-1][x-1].SetValue(door_);
        cells_[y-1][x-1].SetState(cs_Door);
        list_cells_.push_back(cells_[y-1][x-1]);
    }
}

void Field::CreateObstacle(int x,int y)
{
    if (cells_[y-1][x-1].GetState() != cs_Wall)
    {
        cells_[y-1][x-1].SetValue(obstacle_);
        cells_[y-1][x-1].SetState(cs_Obstacle);
    }
}

void Field::Moore()
{
    double currentCellVal;
    while (!list_cells_.empty())
    {
        int currentX = (list_cells_.front()).GetX();
        int currentY = (list_cells_.front()).GetY();
        currentCellVal = cells_[currentY][currentX].GetValue();
        list_cells_.pop_front();
        if (currentX > 0)    SetCell(currentY,currentX-1,currentCellVal + m_s_); // Lewy sasiad
        if (currentX < s_x_) SetCell(currentY,currentX+1,currentCellVal + m_s_); // Prawy sasiad
        if (currentY > 0)    SetCell(currentY-1,currentX,currentCellVal + m_s_); // Gorny sasiad
        if (currentY < s_y_-1) SetCell(currentY+1,currentX,currentCellVal + m_s_); // Dolny sasiad
        if (currentY > 0 && currentX > 0)       SetCell(currentY-1,currentX-1,currentCellVal + m_d_); // Gorny lewy sasiad
        if (currentY > 0 && currentX < s_x_)    SetCell(currentY-1,currentX+1,currentCellVal + m_d_); // Gorny prawy sasiad
        if (currentY < s_y_-1 && currentX > 0)    SetCell(currentY+1,currentX-1,currentCellVal + m_d_); // Dolny lewy sasiad
        if (currentY < s_y_-1 && currentX < s_x_) SetCell(currentY+1,currentX+1,currentCellVal + m_d_); // Dolny prawy sasiad
    }
}

void Field::vonNeumann()
{
    double currentCellVal;
    while (!list_cells_.empty())
    {
        int currentX = (list_cells_.front()).GetX();
        int currentY = (list_cells_.front()).GetY();
        currentCellVal = cells_[currentY][currentX].GetValue();
        list_cells_.pop_front();
        if (currentX > 0)    SetCell(currentY,currentX-1,currentCellVal + m_s_); // Lewy sasiad
        if (currentX < s_x_) SetCell(currentY,currentX+1,currentCellVal + m_s_); // Prawy sasiad
        if (currentY > 0)    SetCell(currentY-1,currentX,currentCellVal + m_s_); // Gorny sasiad
        if (currentY < s_y_-1) SetCell(currentY+1,currentX,currentCellVal + m_s_); // Dolny sasiad
    }
}

void Field::SetCell(int y, int x, double val)
{
    double c_val = cells_[y][x].GetValue();
    int state=cells_[y][x].GetState();
    if(c_val > val && state != cs_Wall && state != cs_Obstacle && state != cs_Door)
    {
        cells_[y][x].SetValue(val);
        list_cells_.push_back(cells_[y][x]);
        cells_[y][x].SetState(cs_Checked);
    }
}

void Field::Evacuation()
{
    Human_out_ *people_out = new Human_out_[numOfp_];	//tablica do zapisu danych wyjsciowych
    
    //zmienne pomocnicze
    Human_out_ tmpHo;
    tmpHo.new_timestep_=0;
    int i=numOfp_; 					//ilosc osob pozostalych w pomieszczeniu (dla while)
    int cX, cY;   					//biezaca pozycja rozpatrywanej osoby
    //-------kopia tablicy people_-------
    Human_ *tpeople = new Human_[numOfp_];
    for (int a=0; a<numOfp_; a++)
	tpeople[a] = people_[a];
    Nei_ minVal;  //uzywane do poszukiwania pola o najmniejszej wartosci z mozliwoscia ruchu
    Nei_ *update = new Nei_[numOfp_]; //uzywane do aktualizacji pozycji
           
    while (i!=0) //dopoki jest ktos w pomieszczeniu
    {
	for (int a=0; a<numOfp_; a++) //sprawdza mozliwosc ruchu kazdej osoby
	{
	    if (tpeople[a].isSafe == false)
	    {
		cX = tpeople[a].x;
		cY = tpeople[a].y;
		//--Zapisywanie pol umozliwiajacych wykonanie ruchu--
	    	if (cX > 0) 	 CheckMove(a, cX, cY, cX-1, cY); //LS
		if (cX < s_x_-1)   CheckMove(a, cX, cY, cX+1, cY); //PS
		if (cY > 0)	 CheckMove(a, cX, cY, cX, cY-1); //GS
		if (cY < s_y_-1) CheckMove(a, cX, cY, cX, cY+1); //DS
		if (cY > 0 && cX > 0) 	      CheckMove(a, cX, cY, cX-1, cY-1, cX-1, cY, cX, cY-1); //GLS 
		if (cY > 0 && cX < s_x_-1)      CheckMove(a, cX, cY, cX+1, cY-1, cX+1, cY, cX, cY-1); //GPS
		if (cY < s_y_-1 && cX > 0)    CheckMove(a, cX, cY, cX-1, cY+1, cX-1, cY, cX, cY+1); //DLS
                if (cY < s_y_-1 && cX < s_x_-1) CheckMove(a, cX, cY, cX+1, cY+1, cX+1, cY, cX, cY+1); //DPS
		//---------------------------------------------------
		
		//sprawdzenie najmniejszej wartosci sposrod komorek mozliwych do ruchu
		if (numOfv_ > 0)
		{ 
		    minVal = vals[0];
		    for(int b=1; b<numOfv_; b++)
			if (vals[b].val < vals[b-1].val) minVal = vals[b];
		    update[a].a = a;
		    update[a].val = minVal.val;
		    update[a].x = minVal.x;
		    update[a].y = minVal.y;
		    cells_[minVal.y][minVal.x].SetOccState(cs_isOccupied);
		    update[a].moved = true;
		    //std::cout << "  a:" << a << "  tpeople[a].x:" << tpeople[a].x << "  update[a].x:"<< update[a].x <<"  tpeople[a].y:" << tpeople[a].y  <<"  update[a].y:"<< update[a].y   << std::endl;
		}
		
		//jesli nie ma mozliwosci ruchu to sprawdzam czy deliwent stoi w drzwiach
		else if (cells_[tpeople[a].y][tpeople[a].x].GetState() == cs_Door) 
		{
		    //std::cout << "i:" << i << "  a:" << a << "  tpeople[a].x:" << tpeople[a].x << "  tpeople[a].y:" << tpeople[a].y << std::endl;
		    i--; 
		    tpeople[a].isSafe = true; 
		    //cells_[tpeople[a].y][tpeople[a].x].SetOccState(cs_isNotOccupied);
		    //tmpHo.old_x = tpeople[a].x;
		    //tmpHo.old_y = tpeople[a].y;
		    update[a].x = 0;
		    update[a].y = 0;
		    //tmpHo.id = tpeople[a].id;
		    //tmpHo.isSafe = tpeople[a].isSafe;
		    //list_out_.push_back(tmpHo);
		    update[a].val = 1;
		    update[a].moved = true;
		}
		
		//jesli nie ma mozliwosci ruchu i nie stoi w drzwiach to znaczy ze sie nie poruszy w tej turze
		else update[a].moved = false;
		
	    }
	    numOfv_ = 0; //zeruje ilosc mozliwosci ruchu
	}
	
	//zapis danych o ruchach na liste wyjsciowa
	for (int a=0; a<numOfp_; a++)
	{
	    if (update[a].moved == true )//&& cells_[update[a].y][update[a].x].GetOccState() != cs_isOccupied)
	    {
		//std::cout << "  a:" << a << "  tpeople[a].x:" << tpeople[a].x << "  update[a].x:"<< update[a].x <<"  tpeople[a].y:" << tpeople[a].y  <<"  update[a].y:"<< update[a].y   << std::endl;
		cells_[tpeople[a].y][tpeople[a].x].SetOccState(cs_isNotOccupied);
		tmpHo.old_x = tpeople[a].x;
		tmpHo.old_y = tpeople[a].y;
		tpeople[a].x = update[a].x;
		tpeople[a].y = update[a].y;
		tmpHo.new_x = tpeople[a].x;
		tmpHo.new_y = tpeople[a].y;
		if (tpeople[a].isSafe == false)
		    cells_[tpeople[a].y][tpeople[a].x].SetOccState(cs_isOccupied);
		tmpHo.id = tpeople[update[a].a].id;
		tmpHo.isSafe = tpeople[a].isSafe;
		list_out_.push_back(tmpHo);
		update[a].moved = false;
	    }   
	}
	tmpHo.new_timestep_++; //wszyscy ktorzy mogli wykonali ruch wiec zwiekszam krok czasu
    }
    tmpHo.old_x = 0;
    tmpHo.old_y = 0;
    tmpHo.new_x = 0;
    tmpHo.new_y = 0;
    tmpHo.id = wall_;
    tmpHo.isSafe = true;
    tmpHo.new_timestep_;
    list_out_.push_back(tmpHo);
    SavePeoplePositions(); // na koniec zapisuje wszystkie ruchy
    delete [] people_out;
    delete [] tpeople;
    delete [] update;
}

void Field::CheckMove(int na, int CX, int CY, int CXp, int CYp)
{
    int occSt = 0;
    occSt = cells_[CYp][CXp].GetOccState();
    if (cells_[CYp][CXp].GetValue() < cells_[CY][CX].GetValue() && occSt != cs_isOccupied)
	SaveValuePosition(CYp,CXp,na);
}

void Field::CheckMove(int na, int CX, int CY, int CXp, int CYp, int CXp1, int CYp1, int CXp2, int CYp2)
{
    int state1, state2;
    int occSt = 0;
    occSt = cells_[CYp][CXp].GetOccState();
    state1 = cells_[CYp1][CXp1].GetOccState();
    state2 = cells_[CYp2][CXp2].GetOccState();
    if (state1 != cs_isOccupied && state2 != cs_isOccupied)
	if (cells_[CYp][CXp].GetValue() < cells_[CY][CX].GetValue() && occSt != cs_isOccupied)
	    SaveValuePosition(CYp,CXp,na);
}

void Field::SaveValuePosition(int Y, int X, int a)
{
    vals[numOfv_].val = cells_[Y][X].GetValue();
    vals[numOfv_].x = X;
    vals[numOfv_].y = Y;
    vals[numOfv_].a = a;
    numOfv_++;
}

void Field::SaveField()
{
    FILE* fl;
    fl=fopen("field","w");
    fprintf(fl,"%d %d\n\n       ",s_x_,s_y_);
    fprintf(fl,"\n\n");
    for(int a=0; a<s_y_; a++)
        for(int b=0; b<s_x_; b++) fprintf(fl,"%6.1f %d %d ",cells_[a][b].GetValue()
                                                           ,cells_[a][b].GetState()
                                                           ,cells_[a][b].GetOccState());
    fprintf(fl,"\n\n");
    for(int a=0; a<numOfp_; a++) fprintf(fl,"%i %i %i\n",people_[a].id, people_[a].x, people_[a].y);  
    fclose(fl);
}

void Field::SavePeoplePositions()
{
    FILE* fl;
    fl=fopen("people_moves","w");
    fprintf(fl,"%d\n",numOfp_);
    while (list_out_.empty() == false)
    {
        fprintf(fl,"%d \n", list_out_.front().id);
        fprintf(fl,"%d \n", list_out_.front().isSafe);
        fprintf(fl,"%d %d \n", list_out_.front().new_x, list_out_.front().new_y);
        fprintf(fl,"%d %d \n", list_out_.front().old_x, list_out_.front().old_y);
        fprintf(fl,"%d \n", list_out_.front().new_timestep_);
	fprintf(fl,"\n");
        list_out_.pop_front();
    }
    fclose(fl);
}
