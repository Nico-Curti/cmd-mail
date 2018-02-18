#include <iostream> // cout
#include <vector> // vector of files to send
#include <string> // substr
#include <map> // yaml config
#include <fstream> // io configfile
#include <algorithm> // find_first_of/find_last_of
#ifdef _WIN32
	#include <conio.h>
	const char sep = '\\';
	#define GetCurrentDir _getcwd
	#define Popen _popen
	#include <Windows.h>
	#include <direct.h>
#else
	const char sep = '/';
	#define GetCurrentDir getcwd
	#define Popen popen
	#include <dirent.h>
	#include <sys/types.h>
	#include <errno.h>
	#include <stdio.h>
	#include <termios.h>
	#include <unistd.h>
	inline int mygetch()
	{
		struct termios oldt, 
					   newt;
		int 		   ch;
		tcgetattr( STDIN_FILENO, &oldt );
		newt = oldt;
		newt.c_lflag &= ~( ICANON | ECHO );
		tcsetattr( STDIN_FILENO, TCSANOW, &newt );
		ch = getchar();
		tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
		return ch;
	}
#endif

typedef std::map<std::string, std::map<std::string, std::string>> config;

inline std::string joinString(const char &sep, const std::string &tk1, const std::string &tk2)
{	return tk1 + sep + tk2;	}
template<typename... Args> inline std::string joinString(const char &sep, const std::string &tk1, const std::string &tk2, Args... args)
{	return tk1 + sep + joinString(sep, tk2, args...);	}

static inline int send_mail(const std::string &smtp,
					  		const std::string &port,
					  		const std::string &from,
					  		const std::string &to,
					  		const std::string &psswrd,
					  		const std::string &msg_file)
{
	std::string cmd = joinString(' ', "curl", "smtp://" + smtp + ":" + port, 
									  "--mail-from", "\"" + from + "\"",
									  "--mail-rcpt", "\"" + to + "\"",
									  "--ssl", "-u", from + ":" + psswrd,
									  "-T", msg_file
								);
#ifdef _WIN32
	int ret = std::system(cmd.c_str());// + " 2> nul").c_str());
#else
	int ret = std::system(cmd.c_str());// + " > /dev/null").c_str());
#endif
	return ret;
}

void parse_yml(const std::string &filename, config &yml)
{
	std::ifstream is(filename);
	if(!is){std::cerr << "Parse Yml error! File not found. Given: " << filename << std::endl; exit(1);}
	std::string row, tag1, tag2, key;
	std::size_t pos;

	while(std::getline(is, row))
	{
		row = row.substr(0, row.find_first_of("#")); // remove comments
		row.erase(0, row.find_first_not_of(" \t")); // trim initial white spaces
		row.erase(row.find_last_not_of(" \t") + 1); // trim final white spaces
		if(row.size() == 0) // check length string
			continue;
		pos = row.find_first_of(":");
		if(pos == row.size() - 1) // if there isn't any values is a key
		{
			key = row.substr(0, pos);
			continue;
		}

		tag1 = row.substr(0, pos);
		tag2 = row.substr(pos + 1, row.size());
		tag2.erase(std::remove(tag2.begin(), tag2.end(), '\''), tag2.end()); // remove double-quotes
		tag2.erase(0, tag2.find_first_not_of(" \t")); // remove initial white spaces
		yml[key][tag1] = tag2;
	}
	is.close();
	return;
}

inline void sender( config &conf,
					std::string &FROM,
					std::string &TO,
					const std::string &cmd
				 	)
{
	std::string msg_file = "mail.tmp",
				script = cmd.substr(0, cmd.find_first_of(" ")),
				password;
	std::cout << "*-* Send Mail *-*" << std::endl;

	std::ofstream os(msg_file);
	os 	<< "From: \""<< conf[FROM]["username"] << "\" <" << conf[FROM]["address"] << ">" << std::endl 
		<< "To: \"" << conf[TO]["username"] << "\" <" << conf[TO]["address"] << ">" << std::endl
		<< "Subject: Results run " << script.substr(script.find_last_of("/\\") + 1) << std::endl
		<< std::endl;

	char ch;
	password = "";
	std::cout << "\rPassword " << conf[FROM]["address"] << " : ";
#ifdef _WIN32
   	while((ch = _getch()) != 13)
   		password += ch;
#else
   	while((ch = mygetch()) != '\n')
   		password += ch;
#endif
   	std::cout << std::endl;
   	os.close();
   	int res = std::system((cmd + " >> " + msg_file).c_str());

   	os.open(msg_file, std::ios_base::app);
   	os << ((res == 0) ? "\nSuccess! Script exit with 0. For the output stream see the logfile." : 
   						"\nSomethig goes wrong. Please see the logfile for more informations." 
   						)
   						<< std::endl;
	os.close();

	std::cout << "Sending e-mail... " << std::endl;
	int dummy = send_mail(conf[FROM]["smtp"], 
						  conf[FROM]["port"],
						  conf[FROM]["address"], 
						  conf[TO]["address"], 
						  password,
						  msg_file	
						  );
	std::cout << ((dummy != 0) ? "[FAILED]" : "[DONE]") << std::endl;

#ifdef _WIN32
	dummy = std::system(("del " + msg_file).c_str());
#else
	dummy = std::system(("rm "  + msg_file).c_str());
#endif
	return;
}

static void show_usage(std::string exe)
{
	std::cerr << "Usage: " << exe << " <option(s)>" << std::endl
		<< "Options:" << std::endl
		<< "\t-h,--help\t\tShow this help message" << std::endl
		<< "\t" << exe << " from@domain.it to@domain.it \"command line\"" << std::endl << std::endl
		<< "Example : " << exe << " " << std::endl
		<< std::endl;
}

int main(int argc, char** argv)
{
	if(argc != 4 || (std::string)argv[1] == "--help" || (std::string)argv[1] == "-h")
	{ 
		std::string exe = (std::string)argv[0];
		show_usage(exe.substr(exe.find_last_of("/\\") + 1)); 
		return 0; 
	}
	std::string from = (std::string)argv[1],
				to = (std::string)argv[2];
	config conf;
	parse_yml("mail.config", conf);

	sender(conf, from, to, (std::string)argv[3]);

	return 0;
}

