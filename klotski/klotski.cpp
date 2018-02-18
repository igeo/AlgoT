// https://zh.wikipedia.org/zh-cn/%E8%8F%AF%E5%AE%B9%E9%81%93_(%E9%81%8A%E6%88%B2)
// http://simonsays-tw.com/web/Klotski/Klotski.html
// 

#include <algorithm>
#include <iostream>

#define ROWs {0,1,2,3,4}
#define COLs {0,1,2,3}

struct State : public std::vector<std::string>
{
// 4 K means the King
// V
// v  means vertical   knight
// Hh means horizontal knight
// P is pawn
// space is empty
   State() : std::vector<std::string>({
    "VKKV",
    "vKKv",
    "VHhV",
    "v  v",
    "PPPP"}) {};
   void print() const;
   void printRepr() const;
   bool hasWon() const { return (*this)[4][1] == 'K' && (*this)[4][2] == 'K';}
   std::vector<std::pair<char,char>> holes() const; // hole position
   std::vector<State> moves() const;
   State getMirror() const;
};

std::vector<std::pair<char,char>> State::holes() const
{
    std::vector<std::pair<char,char>> ret;
    for(char y : ROWs)
       for(char x : COLs) 
           if((*this)[y][x] == ' ')
           {
               ret.emplace_back(std::make_pair(y, x));
               if(ret.size() == 2)
               {
                   //std::cout << "hole: " << int(ret[0].first) << int(ret[0].second) << int(ret[1].first) << int(ret[1].second) << std::endl;
                   return ret;
               }
           }

}

std::vector<State> State::moves() const
{
    std::vector<State> ret;
    const auto hs = holes();
    for(const auto h : hs)
    {
        const auto y = h.first;
        const auto x = h.second;
        if(y > 0) // move down
        {
            if((*this)[y-1][x] == 'P')
            {
                State s = *this;
                std::swap(s[y][x], s[y-1][x]);
                ret.emplace_back(s);
            }
            else
            {
                if((*this)[y-1][x] == 'v')
                {
                   State s = *this;
                   std::swap(s[y][x], s[y-1][x]);
                   std::swap(s[y-2][x], s[y-1][x]);
                   ret.emplace_back(s);
                }
            }
        }

        if(y < 4) // move up
        {
            if((*this)[y+1][x] == 'P')
            {
                State s = *this;
                std::swap(s[y][x], s[y+1][x]);
                ret.emplace_back(s);
            }
            else
            {
                if((*this)[y+1][x] == 'V')
                {
                   State s = *this;
                   std::swap(s[y][x], s[y+1][x]);
                   std::swap(s[y+2][x], s[y+1][x]);
                   ret.emplace_back(s);
                }
            }
        }

        if(x > 0) // move right
        {
            if((*this)[y][x-1] == 'P')
            {
                State s = *this;
                std::swap(s[y][x], s[y][x-1]);
                ret.emplace_back(s);
            }
            else
            {
                if((*this)[y][x+1] == 'h')
                {
                   State s = *this;
                   std::swap(s[y][x], s[y][x-1]);
                   std::swap(s[y][x-2], s[y][x-1]);
                   ret.emplace_back(s);
                }
            }
        }

        if(x < 3) // move left
        {
            if((*this)[y][x+1] == 'P')
            {
                State s = *this;
                std::swap(s[y][x], s[y][x+1]);
                ret.emplace_back(s);
            }
            else
            {
                if((*this)[y][x+1] == 'H')
                {
                   State s = *this;
                   std::swap(s[y][x], s[y][x+1]);
                   std::swap(s[y][x+2], s[y][x+1]);
                   ret.emplace_back(s);
                }
            }
        }
    }
    std::cout << "move size: " << ret.size() << std::endl;
    return ret;
}

void State::printRepr() const
{
    std::cout << std::endl;
   for(char y : ROWs) // process each row
       std::cout << (*this)[y] << std::endl;
}
void State::print() const
{
   const int hs = 2; // H size
   const int vs = 1;
   std::cout << '+' << std::string(4*(hs+1), '-') << std::endl;
   for(char y : ROWs) // process each row
   {
     for(char v : std::vector<int>(vs, 0)) // height of a row
     {
       std::cout << '|';
       for(char x : COLs)
       {
            char disp = ' ';
            if((*this)[y][x] == ' ')
                disp = '#';
            std::cout << std::string(hs, disp);
            if ((*this)[y][x] == 'H' || ((*this)[y][x] == 'K' && x < 3 && (*this)[y][x+1] == 'K'))
                std::cout << ' ';
            else
                std::cout << '|';
       }
       std::cout << std::endl;
     }
     if ( y < 4 ) // horization lines between
     {
       std::cout << '|';
       for(char x : COLs)
       {
            if ((*this)[y][x] == 'V' ||  ((*this)[y][x] == 'K' && (*this)[y+1][x] == 'K'))
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
    s.holes();
    const auto ms = s.moves();
    for (const auto m : ms)
        m.printRepr();
    return 0;
}
