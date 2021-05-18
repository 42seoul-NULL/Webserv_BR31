#ifndef SINGLETON_HPP
# define SINGLETON_HPP

# include <stdlib.h>

class config
{
    private :
        config(){ this->port = 0; }      // 기본 생성자를 숨겨준다.
		config(const config& origin){}
		config& operator=(const config& origin)	{return (*this);}

		static config*	instance;
		int		port;
	public	:
		virtual ~config()
		{
			delete instance;
		}

		static config* getInstance()
		{
			if (instance == NULL)
				instance = new config();
			return (instance);
		}
		
		void	setPort(int value)
		{
			this->port = value;
		}

		int		getPort(void)
		{
			return (this->port);
		}
};

config* config::instance; // 참고 https://stackoverflow.com/questions/16284629/undefined-reference-to-static-variable-c

#endif