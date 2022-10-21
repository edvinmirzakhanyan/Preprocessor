#include <string>
#include <map>
#include <vector>
#include <fstream>

class PP
{
    public:
        PP(const std::string&);
    private:
        void _parse(std::string&);
        void _include(const std::string&);
        void _define(std::vector<std::string>&);
        void _ifndef(const std::string&);
        void _ifdef(const std::string&); 
        void _file_read(std::ifstream&);
    private:
        std::map<std::string, std::string> symtab;
        std::vector<std::string> deftab;
        bool def_flag = true;
};

PP::PP(const std::string& file)
{
    _include(file);
}

void PP::_parse(std::string& str)
{
    int size = str.size();
    int sp_count = 1;
    int sym_count = 0;


    for (int i = 0; i < size; ++i)
    {
        if (str[i] == '"')
        { 
            ++sym_count;
            continue; 
        }

        if (sym_count % 2 == 0 && str[i] != ' ' && str[i + sp_count] == ' ')
        {
            ++sp_count;
            --i;
            continue;
        }
        if (sp_count > 1)
        {
            str.erase(i + 1, sp_count - 2);
            size -= sp_count - 2;
            sp_count = 1;
        } 
    }
}

void PP::_include(const std::string& str)
{
    std::ifstream fin;
    fin.open(str);
    _file_read(fin);
}

void PP::_define(std::vector<std::string>& symbol)
{
    if (symbol.size() == 3)
    {
        symtab[symbol[1]] = symbol[2];
        return;
    }
    else if (symbol.size() == 2)
    {
        deftab.push_back(symbol[1]);
    }
}

void PP::_ifndef(const std::string& def)
{
    if (std::count(deftab.begin(), deftab.end(), def)) { def_flag = false; }
    else { def_flag = true; }
}

void PP::_ifdef(const std::string& def)
{
    if (std::count(deftab.begin(), deftab.end(), def)) { def_flag = true; }
    else { def_flag = false; }
}

void PP::_file_read(std::ifstream& fin)
{   
    while (!fin.eof())
    {
        std::vector<std::string> symbol;
        std::string word;
        symbol.push_back(word);
        
        std::ofstream fout;
        fout.open("textoutput.txt", std::ofstream::app);
        
        std::string tmp;
        getline(fin, tmp);
        _parse(tmp);
        int size = tmp.size();

        for (int i = 0, j = 0; i < size; ++i)
        {
            if (symbol[j].size() > 0 && tmp[i] == ' ')
            { 
                symbol.push_back(word);
                ++j;
            }
            else { symbol[j] += tmp[i]; }
            
            if (symtab.contains(symbol[j]))
                {
                    tmp.erase(i - symbol[j].size() + 1, symbol[j].size());
                    tmp.insert(i - symbol[j].size() + 1, symtab[symbol[j]]);
                }
        }

        if (symbol[0] == "#include")
        {
            fout.close();
            _include(symbol[1]);
            continue;
        }
        else if (symbol[0] == "#define")
        {
            _define(symbol);
            continue;
        }
        else if (symbol[0] == "#ifndef")
        {
            _ifndef(symbol[1]);
            continue;
        }
        else if (symbol[0] == "#ifdef")
        {
            _ifdef(symbol[1]);
            continue;
        }
        else if (symbol[0] == "#endif") 
        { 
            def_flag = true;
            continue;
        }
        if (def_flag) { fout << tmp + '\n';}
    }
    fin.close();
}
