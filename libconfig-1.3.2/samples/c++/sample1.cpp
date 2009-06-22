/*************************************************************************
 ** Sample1
 ** Load sample.cfg and increment the "X" setting
 *************************************************************************/

#include <iostream>
#include <libconfig.h++>

using namespace libconfig;
using namespace std;

/***************************************************************************/

int main()
{
  Config cfg;

  try
  {
    /* Load the configuration.. */
    cout << "loading [sample.cfg]..";
    cfg.readFile("sample.cfg");
    cout << "ok" << endl;

    /* Increment "x" */
    cout << "increment \"x\"..";
    Setting& s = cfg.lookup("x");
    long x = s;
    s = ++x;
    cout << "ok (x=" << x << ")" << endl;

    // Save the configuration
    cout << "saving [sample.cfg]..";
    cfg.writeFile("sample.cfg");
    cout << "ok" << endl;

    cout << "Done!" << endl;
  }
  catch (...)
  {
    cout << "failed" << endl;
  }

  return 0;
}

/***************************************************************************/
