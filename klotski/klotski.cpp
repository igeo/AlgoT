// https://zh.wikipedia.org/zh-cn/%E8%8F%AF%E5%AE%B9%E9%81%93_(%E9%81%8A%E6%88%B2)
// http://simonsays-tw.com/web/Klotski/Klotski.html
// http://blog.csdn.net/mu399/article/details/78499524
// http://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html

// https://nbickford.wordpress.com/2013/11/13/sliding-block-puzzles-part-4-of-3/#more-371 this give the most conprehenive review I ever seen. 
// I will check if there is anything he missed.

#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <map>
#include <chrono>
#include  <cstring>

#define ROWs {0,1,2,3,4}
#define COLs {0,1,2,3}

// use vector<sting> to represnt state
struct StateBase_VS : public std::vector<std::string>
{
    StateBase_VS() : std::vector<std::string>(
    {
        "VKKV",
        "vKKv",
        "VHhV",
        "vPPv",
        "P  P"}) {};
    StateBase_VS(const std::vector<std::string>&  s) : std::vector<std::string>::vector(s) {};
};

// State based on array of array
struct StateBase_AA : public std::array<std::array<char,4>,5>
{
    StateBase_AA() : std::array<std::array<char,4>,5>({{
            {'V', 'K', 'K', 'V'},
            {'v', 'K', 'K', 'v'},
            {'V', 'H', 'h', 'V'},
            {'v', 'P', 'P', 'v'},
            {'P', ' ', ' ', 'P'}}}) {};
    StateBase_AA(const std::vector<std::string>&  s);
};

StateBase_AA::StateBase_AA(const std::vector<std::string>& s)
{
    for(char y : ROWs) // process each row
        for(char x : COLs)
            (*this)[y][x] = s.at(y).at(x);
}

/// Statue represnt the status of the klotski board
// 4 K means the King
// V
// v  means vertical   knight
// Hh means horizontal knight
// P is pawn
// space is empty
#if 0
struct State : public StateBase_VS
{
    State() : StateBase_VS() {};
    State(const std::vector<std::string>&  s) : StateBase_VS(s) {};
#else
struct State : public StateBase_AA
{
    State() : StateBase_AA() {};
    State(const std::vector<std::string>&  s) : StateBase_AA(s) {};
#endif
    char& L(char x, char y)       { if(x<1) return out; return (*this)[y][x-1]; } // left
    const char& L(char x, char y) const { if(x<1) return out; return (*this)[y][x-1]; }
    char& R(char x, char y)       { if(x>2) return out; return (*this)[y][x+1]; } // right
    const char& R(char x, char y) const { if(x>2) return out; return (*this)[y][x+1]; }
    char& U(char x, char y)       { if(y<1) return out; return (*this)[y-1][x]; } // up
    const char& U(char x, char y) const { if(y<1) return out; return (*this)[y-1][x]; }
    char& D(char x, char y)       { if(y>3) return out; return(*this)[y+1][x]; } // down
    const char& D(char x, char y) const { if(y>3) return out; return(*this)[y+1][x]; }
    char& C(char x, char y)       { return (*this)[y][x]; } // self
    const char& C(char x, char y) const { return (*this)[y][x]; }

    void swapWith(char x, char y, char direction, char size); // move on step

    void print() const;
    void printRepr() const;
    bool hasWon() const { return (*this)[4][1] == 'K' && (*this)[4][2] == 'K';}
    std::array<std::pair<char,char>,2> holes() const; // hole position
    const std::vector<State>& moves() const;
    State getMirror() const;
    const unsigned long long getHashableL() const;
    const std::string getHashable() const;

    static char out; // to avoid handling of out of range
};
char State::out = '@';


const unsigned long long State::getHashableL() const
{
    unsigned long long buf = 0;
    //for(const auto y : ROWs) for(const auto x : COLs) switch(C(x,y))
    const char* p = (*this).front().data();
    for(int i = 0; i < 20; ++i)
    {
        buf <<= 3;
        switch(*p++)
        {
            case ' ':
                buf |= 0x0;
                break;
            case 'K':
                buf |= 0x1;
                break;
            case 'H':
                buf |= 0x2;
                break;
            case 'h':
                buf |= 0x3;
                break;
            case 'V':
                buf |= 0x4;
                break;
            case 'v':
                buf |= 0x5;
                break;
            case 'P':
                buf |= 0x6;
        }
    }
    return buf;
}

const std::string State::getHashable() const
{
    std::string dig(20, ' ');
    for(auto y : ROWs)
        for(auto x : COLs)
            dig[x+y*4] = C(x,y);
    return dig;
}

std::array<std::pair<char,char>,2> State::holes() const
{
    std::array<std::pair<char,char>,2> ret;
    int count = 0;
    for(char y : ROWs)
        for(char x : COLs)
            if(C(x,y) == ' ')
            {
                ret[count++] = std::make_pair(y, x);
                if(count > 1)
                    return ret;
            }
    return ret;
}

bool debug = false;


State State::getMirror() const
{
    auto m = *this;
    for(auto y : ROWs)
    {
        for(char x : {0,1})
            std::swap(m.C(x,y), m.C(3-x,y));
        size_t pos = 0;
        for(char x : {1, 2, 3})
            if(m.C(x,y) == 'H')
                m.swapWith(x,y, 'l', 1);
    }
    return m;
}

const std::vector<State>& State::moves() const
{
    static std::vector<State> ret;
    ret.clear();
    //ret.reserve(10);
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
            s.swapWith(x,y, 'u', 1);
            ret.emplace_back(s); // P down
        }
        else
        {
            if(U(x,y) == 'v')
            {
                State s = *this;
                s.swapWith(x,y, 'u', 1);
                s.swapWith(x,y-1, 'u', 1);
                ret.emplace_back(s); // V down
            }
        }

        // move up
        if(D(x,y) == 'P')
        {
            State s = *this;
            s.swapWith(x,y, 'd', 1);
            ret.emplace_back(s); // P up
        }
        else
        {
            if(D(x,y) == 'V')
            {
                State s = *this;
                s.swapWith(x,y, 'd', 1);
                s.swapWith(x,y+1, 'd', 1);
                ret.emplace_back(s); // V up
            }
        }

        // move right
        if(L(x,y) == 'P')
        {
            State s = *this;
            s.swapWith(x,y, 'l', 1);
            ret.emplace_back(s);
            if(debug) std::cout << "P r" << std::endl;
        }
        else
        {
            if(L(x,y) == 'h')
            {
                State s = *this;
                s.swapWith(x,y, 'l', 1);
                s.swapWith(x-1,y, 'l', 1);
                ret.emplace_back(s);
                if(debug) std::cout << "H r" << std::endl;
            }
        }

        // move left
        if(R(x,y) == 'P')
        {
            State s = *this;
            s.swapWith(x,y, 'r', 1);
            ret.emplace_back(s);
            if(debug) std::cout << "P l" << std::endl;
        }
        else
        {
            if(R(x,y) == 'H')
            {
                State s = *this;
                s.swapWith(x,y, 'r', 1);
                s.swapWith(x+1,y, 'r', 1);
                ret.emplace_back(s);
                if(debug) std::cout << "H l" << std::endl;
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
            if(debug) std::cout << "K down" << std::endl;
        }
        if(D(x1,y) == 'K' && D(x2,y) == 'K')
        {
            State s = *this;
            s.swapWith(x1,y, 'd', 2);
            s.swapWith(x1,y+1, 'd', 2);
            ret.emplace_back(s);
            if(debug) std::cout << "K up" << std::endl;
        }
        if(U(x1,y) == 'H')
        {
            State s = *this;
            s.swapWith(x1,y, 'u', 2);
            ret.emplace_back(s);
            if(debug) std::cout << "H down" << std::endl;
        }
        if(D(x1,y) == 'H')
        {
            State s = *this;
            s.swapWith(x1,y, 'd', 2);
            ret.emplace_back(s);
            if(debug) std::cout << "H up" << std::endl;
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
            if(debug) std::cout << "K r" << std::endl;
        }
        if(R(x,y1) == 'K' && R(x,y2) == 'K')
        {
            State s = *this;
            s.swapWith(x,y1, 'r', 2);
            s.swapWith(x+1,y1, 'r', 2);
            ret.emplace_back(s);
            if(debug) std::cout << "K r" << std::endl;
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
            case 'u':
                t = -1;
                std::swap(C(x+d,y), C(x+d,y+t));
                break;
            case 'd':
                t = +1;
                std::swap(C(x+d,y), C(x+d,y+t));
                break;
            case 'l':
                t = -1;
                std::swap(C(x,y+d), C(x+t,y+d));
                break;
            case 'r':
                t = +1;
                std::swap(C(x,y+d), C(x+t,y+d));
                break;
        }
}

std::string colorfy(const char c)
{
    const std::map<char,std::string> cm
    {
        {' ', u8"\u001b[40m"}, // black space
        {'k', u8"\u001b[41m"}, // red king
        {'h', u8"\u001b[42m"}, // green H
        {'v', u8"\u001b[44m"}, // blue V
        {'p', u8"\u001b[46m"}}; // Cyan P

    return cm.at(tolower(c)) + c;
}

void State::printRepr() const
{
    const std::string n = u8"\u001b[0m"; // back to normal color
    std::cout << std::endl;
    for(char y : ROWs) // process each row
    {
        for(char x : COLs)
            std::cout << C(x,y);
        std::cout << '|' << std::endl;
    }
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

std::vector<State> back_trace(const std::vector<std::vector<std::pair<State,size_t>>>& progress)
{
    std::vector<State> solution;
    State s;// = progress.back().back().first;
    size_t idx= progress.back().size()-1;
    progress.at(0).at(0).first;
    for(int depth = progress.size() - 1; depth >= 0; --depth)
    {
        const auto& front = progress.at(depth);
        const auto& node = front.at(idx);
        solution.push_back(node.first);
        idx = node.second;
    }
    std::reverse(solution.begin(), solution.end());
    return solution;
}


std::vector<State> solve(const State& start)
{
    std::vector<std::vector<std::pair<State,size_t>>> progress; // each vector is a step, index is parent location
    progress.emplace_back(std::vector<std::pair<State,size_t>>(1, std::make_pair(start,0)));
    //std::vector<std::vector<std::pair<unsigned long long,size_t>>> progress; // each vector is a step, index is parent location
    //progress.emplace_back(std::vector<std::pair<unsigned long long,size_t>>(1, std::make_pair(start.getHashableL(),0)));
    std::unordered_set<unsigned long long> seen;
    std::unordered_set<std::string> seenS;

    progress.reserve(300);
    seen.rehash(40000);

    for(size_t i = 0; i < 300; ++i)
    {
        if(debug) std::cout << "==== Running step # " << i + 1 << " ====" << std::endl;
        progress.emplace_back(std::vector<std::pair<State,size_t>>());
        auto& next = progress.back();  // new depth to explore
        next.reserve(300);
        const auto& current = progress[i]; // current depth
        for(size_t leaf_idx = 0; leaf_idx < current.size(); ++leaf_idx) // loop all current leafs
        {
            const auto& ms = current[leaf_idx].first.moves(); // what is poositble next move
            for(const auto& m : ms)
            {
                auto hashable = m.getHashableL();
                if(!seen.insert(hashable).second || !seen.insert(m.getMirror().getHashableL()).second)
                    continue; // donot loop

                next.emplace_back(std::make_pair(m, leaf_idx));
                if(m.hasWon())
                {
                    std::cout << "*** Solved at step " << i+1 << std::endl;
                    m.printRepr();
                    int N = 0;
                    for(const auto& v : progress)
                        N += v.size();
                    std::cout << "total serched size " << N << std::endl;
                    return back_trace(progress);
                }
            }
        }
        if(debug) std::cout << "==== total leafs " << next.size() << " ====" << std::endl;
        if(next.empty())
        {
            std::cout << "did not find solution after step " << i+1 << std::endl;
            break;
        }
    }
    return std::vector<State>();
}

int main(int argc, char** argv)
{
    State t1 (
    {
        "HhV ",
        "Hhv ",
        "VKKV",
        "vKKv",
        "PPPP"});
    t1.moves();

    State t2 (
    {
        "VHhV",
        "vHhv",
        "KK V",
        "KK v",
        "PPPP"});

    State t3 (  // unsoverable
    {
        "VKKV",
        "vKKv",
        "VVPV",
        "vvPv",
        "P  P"
    });

    State s = State();
    //s = t3;
    s.print();
    s.printRepr();
    auto start = std::chrono::system_clock::now();
    auto solution = solve(s);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

    if(argc >= 2 && std::string(argv[1]) == "-bt")
    {
        std::cout << "solution:  " << std::endl;
        for(size_t i = 0; i < solution.size(); ++i)
        {
            std::cout << " step #" << i << std::endl;
            solution[i].print();
        }
    }
    return 0;
}
