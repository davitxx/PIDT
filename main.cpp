#include "field.h"

int main()
{
    const char *f = "input_data";       // Input data file - Floor diementions
    const char *pf = "people_data";     // Input data file - People positions

    Field fld;
    fld.InitializeField(f);

    fld.Moore();
    //fld.vonNeumann();
    fld.PeoplePositions(pf);
    fld.Evacuation();

    return 0;
}
