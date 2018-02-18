// https://zh.wikipedia.org/zh-cn/%E8%8F%AF%E5%AE%B9%E9%81%93_(%E9%81%8A%E6%88%B2)
// http://simonsays-tw.com/web/Klotski/Klotski.html
// 

#include <algorithm>
#include <iostream>
#include <unordered_set>

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
   State(const std::vector<std::string>&  s) : std::vector<std::string>::vector(s) {};
         char& L(char x, char y)       { return x-1 < 0 ? out : (*this)[y][x-1]; } // left
   const char& L(char x, char y) const { return x-1 < 0 ? out : (*this)[y][x-1]; } 
         char& R(char x, char y)       { return x+1 > 3 ? out : (*this)[y][x+1]; } // right
   const char& R(char x, char y) const { return x+1 > 3 ? out : (*this)[y][x+1]; }
         char& U(char x, char y)       { return y-1 < 0 ? out : (*this)[y-1][x]; } // up
   const char& U(char x, char y) const { return y-1 < 0 ? out : (*this)[y-1][x]; }
         char& D(char x, char y)       { return y+1 > 4 ? out : (*this)[y+1][x]; } // down
   const char& D(char x, char y) const { return y+1 > 4 ? out : (*this)[y+1][x]; }
         char& C(char x, char y)       { return (*this)[y][x]; } // self 
   const char& C(char x, char y) const { return (*this)[y][x]; }

   void swapWith(char x, char y, char direction, char size); // move on step

   void print() const;
   void printRepr() const;
   bool hasWon() const { return (*this)[4][1] == 'K' && (*this)[4][2] == 'K';}
   std::vector<std::pair<char,char>> holes() const; // hole position
   std::vector<State> moves() const;
   State getMirror() const;
   static char out;
};
   char State::out = '@';

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
    //std::cout << "line:" << __LINE__ << std::endl;
    std::vector<State> ret;
    const auto hs = holes();
    if(hs.size() != 2)
    {
        printRepr();
        std::cout << "hole sizes" << hs.size() << std::endl;
    }

    for(const auto h : hs) // single move
    {
        const auto y = h.first;
        const auto x = h.second;
        // move down
        if(U(x,y) == 'P')
        {
            State s = *this;
            std::swap(s[y][x], s[y-1][x]);
            ret.emplace_back(s);
        }
        else
        {
            //if((*this)[y-1][x] == 'v')
            if(U(x,y) == 'v')
            {
               State s = *this;
               std::swap(s[y][x], s[y-1][x]);
               std::swap(s[y-2][x], s[y-1][x]);
               ret.emplace_back(s);
            }
        }

        // move up
        if(D(x,y) == 'P')
        {
            State s = *this;
            std::swap(s[y][x], s[y+1][x]);
            ret.emplace_back(s);
        }
        else
        {
            if(D(x,y) == 'V')
            {
               State s = *this;
               std::swap(s[y][x], s[y+1][x]);
               std::swap(s[y+2][x], s[y+1][x]);
               ret.emplace_back(s);
            }
        }

        // move right
        if(L(x,y) == 'P')
        {
            State s = *this;
            s.swapWith(x,y, 'l', 1);
            ret.emplace_back(s);
        }
        else
        {
            if(L(x,y) == 'h')
            {
               State s = *this;
               s.swapWith(x,y, 'l', 1);
               s.swapWith(x-1,y, 'l', 1);
               ret.emplace_back(s);
            }
        }

        // move left
        if(R(x,y) == 'P')
        {
            State s = *this;
            s.swapWith(x,y, 'r', 1);
            ret.emplace_back(s);
        }
        else
        {
            if(R(x,y) == 'H')
            {
               State s = *this;
               s.swapWith(x,y, 'r', 1);
               s.swapWith(x+1,y, 'r', 1);
               ret.emplace_back(s);
            }
        }
    }
    
    // double move
    if(hs[0].first == hs[1].first && std::abs(hs[0].second - hs[1].second) == 1)
    {
        const char y = hs[0].first;
        const char x1 = std::min(hs[0].second, hs[1].second);
        const char x2 = x1 + 1;
        // vertical move
        if(U(x1,y) == 'K' && U(x2,y) == 'K')
        {
           State s = *this;
           s.swapWith(x1,y, 'u', 2);
           s.swapWith(x1,y-1, 'u', 2);
           ret.emplace_back(s);
        }
        if(D(x1,y) == 'K' && D(x2,y) == 'K')
        {
           State s = *this;
           s.swapWith(x1,y, 'd', 2);
           s.swapWith(x1,y+1, 'd', 2);
           ret.emplace_back(s);
        }
        if(U(x1,y) == 'H')
        {
           State s = *this;
           s.swapWith(x1,y, 'u', 2);
           ret.emplace_back(s);
        }
        if(D(x1,y) == 'H')
        {
           State s = *this;
           s.swapWith(x1,y, 'd', 2);
           ret.emplace_back(s);
        }

    }
    // double move
    if(hs[0].second == hs[1].second && std::abs(hs[0].first - hs[1].first) == 1)
    {
        const char x = hs[0].second;
        const char y1 = std::min(hs[0].first, hs[1].first);
        const char y2 = y1 + 1;
        // Horizontal  move
        if(L(x,y1) == 'K' && L(x,y2) == 'K')
        {
           State s = *this;
           s.swapWith(x,y1, 'l', 2);
           s.swapWith(x-1,y1, 'l', 2);
           ret.emplace_back(s);
        }
        if(R(x,y1) == 'K' && R(x,y2) == 'K')
        {
           State s = *this;
           s.swapWith(x,y1, 'd', 2);
           s.swapWith(x+1,y1, 'd', 2);
           ret.emplace_back(s);
        }
        if(L(x,y1) == 'V')
        {
           State s = *this;
           s.swapWith(x,y1, 'l', 2);
           ret.emplace_back(s);
        }
        if(R(x,y1) == 'V')
        {
           State s = *this;
           s.swapWith(x,y1, 'r', 2);
           ret.emplace_back(s);
        }

    }

    //std::cout << "move size: " << ret.size() << std::endl;
    if(ret.empty())
    {
        printRepr();
        exit(1);
    }
    return ret;
}

// vertical move size is in x+
// for horizatonal move size in in y+
void State::swapWith(char x, char y, char dir, char size) // move pices on step
{
  char t = 0; // target
  for(char d = 0; d < size; ++d)
    switch (dir)
    {
        case 'u': t = -1;
            std::swap(C(x+d,y), C(x+d,y+t));
            break;
        case 'd': t = +1;
            std::swap(C(x+d,y), C(x+d,y+t));
            break;
        case 'l': t = -1;
            std::swap(C(x,y+d), C(x+t,y+d));
            break;
        case 'r': t = +1;
            std::swap(C(x,y+d), C(x+t,y+d));
            break;
    }
}

void State::printRepr() const
{
    std::cout << std::endl;
   for(char y : ROWs) // process each row
       std::cout << (*this)[y] << "|"<< std::endl;
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
    State test ({
    "HhV ",
    "Hhv ",
    "VKKV",
    "vKKv",
    "PPPP"});
    test.moves();

    State s ({
    "VHhV",
    "vHhv",
    "KK V",
    "KK v",
    "PPPP"});

    s.print();

    s = State();

    std::vector<std::vector<State>> progress; // each vector is a step
    progress.emplace_back(std::vector<State>(1, s));
    std::unordered_set<std::string> seen;

    for(size_t i = 0; i < 100; ++i)
    {
        std::cout << "==== Running step # " << i + 1 << " ====" << std::endl;
        progress.emplace_back(std::vector<State>());
        auto& next = progress.back();
        const auto& now = progress[i];
        for(const auto& p : now) // loop all current leafs 
        {
            //p.printRepr(); std::cout << "...==" << std::endl;
            auto ms = p.moves(); // what is poositble next move
            for(const auto& m : ms)
            {
                std::string dig;
                for(auto i : ROWs)
                    dig += m[i];
                m.printRepr();
                std::cout << "diguest " << dig << std::endl;
                if(seen.count(dig))
                    continue;
                seen.emplace(dig);
                next.emplace_back(m);
                if(m.hasWon())
                {
                   std::cout << "** WON at step " << i+1 << std::endl;
                   m.printRepr();
                   exit(0);
                }
            }
            //std::cout << "^^^" << std::endl;
        }
        std::cout << "==== total leafs " << next.size() << " ====" << std::endl;
    }
    return 0;
}
