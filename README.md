| **Authors**                                   | **Project**     |
|:---------------------------------------------:|:---------------:|
| [**N. Curti**](https://github.com/Nico-Curti) |   **cmd-mail**  |


# SMail :grin:
### Send eMail

The project includes different ways (in different languages) to send email by using simple command line instructions.

Before use this package, please edit the `mail.config` file with your credentials.

1. [Prerequisites](#prerequisites)
2. [Installation](#installation)
3. [Authors](#authors)
4. [License](#license)

## Prerequisites

In every script the function `smail` parse the configuration file [mail.config](https://github.com/Nico-Curti/cmd-mail/blob/master/mail.config) in which informations about users and domains are stored.

If you are using the Python version install the full list of prerequisites using `pip install -r requirements.txt` in the project directory.

All the other smile versions have no particular requirements.
You can compile the C++ version with the simple command

```
g++ smail.cpp -o smail -O3
```

## Languages

The `smail` function is supported in:

- [bash](https://github.com/Nico-Curti/cmd-mail/blob/master/smail.sh)

```bash
bash smail.sh to from "echo Hello World!"
```

- [powershell](https://github.com/Nico-Curti/cmd-mail/blob/master/smail.ps1)


```pwsh
./smail.ps1 -TO to -FROM from -cmd "echo Hello World!"
```

- [python](https://github.com/Nico-Curti/cmd-mail/blob/master/smail.py)

```bash
python smail.py -t to -f from -x "echo Hello World!"
```

- [c++](https://github.com/Nico-Curti/cmd-mail/blob/master/smail.cpp)


```bash
g++ smail.cpp -o smail -O3
./smail to from "echo Hello World!"
```

The mail credentials in the configuration file are parsed and the command line was built using that informations. In addition you can add a command line to execute before the mail sending. In this way the output of the command was redirected into a log file which will be attached to mail.
It can be useful when you are working on long pipeline and you want know when it finish.

## Authors

* **Nico Curti** [git](https://github.com/Nico-Curti), [unibo](https://www.unibo.it/sitoweb/nico.curti2)

## License

The `cmd-mail` package is released under GPL License. [![License](https://img.shields.io/badge/license-GPL-blue.svg?style=plastic)](https://github.com/Nico-Curti/rFBP/blob/master/LICENSE.md)
