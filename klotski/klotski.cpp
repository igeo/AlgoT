// https://zh.wikipedia.org/zh-cn/%E8%8F%AF%E5%AE%B9%E9%81%93_(%E9%81%8A%E6%88%B2)
// http://simonsays-tw.com/web/Klotski/Klotski.html
// 

#include <algorithm>
#include <iostream>


struct State
{
// 4 K means the King
// V
// v  means vertical   knight
// Hh means horizontal knight
// P is pawn
// space is empty
   std::vector<std::string> state {
    "VKKV",
    "vKKv",
    "VHhV",
    "v  v",
    "PPPP"};
   void print();

};

void State::print()
{
   const int hs = 8; // H size
   const int vs = 4;
   std::cout << ' ' << std::string(4*(hs+1), '-') << std::endl;
   for(size_t y = 0; y < 5; ++y) // process each row
   {
     for(const auto v : std::vector<int>(vs, 0)) // height of a row
     {
       std::cout << '|';
       for(size_t x = 0; x < 4; ++x)
       {
            char disp = ' ';
            if(state[y][x] == ' ')
                disp = '#';
            std::cout << std::string(hs, disp);
            if (state[y][x] == 'H' || (state[y][x] == 'K' && x < 3 && state[y][x+1] == 'K'))
                std::cout << ' ';
            else
                std::cout << '|';
       }
       std::cout << std::endl;
     }
     if ( y < 4 ) // horization lines between
     {
       std::cout << '|';
       for(size_t x = 0; x < 4; ++x)
       {
            if (state[y][x] == 'V' ||  (state[y][x] == 'K' && state[y+1][x] == 'K'))
                std::cout << std::string(hs+1,' ');
            else
                std::cout << std::string(hs+1, '-');
       }
       std::cout << std::endl;
     }
   }
   std::cout << ' ' << std::string(4*(hs+1), '-') << std::endl;
}

int main()
{
    State s;
    s.print();
    return 0;
}
