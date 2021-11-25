#include <string>
#include <iostream>

class BaseException
{
protected:
	// объ€снение к исключению - общее дл€ всех производных классов
	std::string text;

public:
	// чиста€ виртуальна€ функци€, котора€ выводит текст исключени€
	virtual std::string what() = 0;
};

class PKCSExceptions : public BaseException {
public:
	PKCSExceptions()
	{
		text = "Error. PKCS Exception.";
	}

	// конструктор с заданным текстом
	PKCSExceptions(std::string _text) { text = _text; }

	// переопредел€ем виртуальную функцию what() - об€зательно
	std::string what() { return text; }
};

class RetVal final :public PKCSExceptions {
public:
	// конструктор, который содержит текст описывающий данное исключение
	RetVal(int RV) : PKCSExceptions()
	{
		text = "Return value is not CKR_OK. Code " + std::to_string(RV); // текст по умолчанию
	}

	std::string what()
	{
		return text;
	}
};

class LibLoadErr final : public PKCSExceptions {
public:
	LibLoadErr() : PKCSExceptions()
	{
		text = "Error loading library.";
	}

	std::string what() 
	{
		return text;
	}
};

class FuncLoadErr final : public PKCSExceptions {
public:
	FuncLoadErr() : PKCSExceptions()
	{
		text = "Error loading function.";
	}

	std::string what()
	{
		return text;
	}
};

class FuncListErr final : public PKCSExceptions {
public:
	FuncListErr() : PKCSExceptions() 
	{
		text = "CK_FUNCTION_LIST_PTR == NULL.";
	}
	std::string what()
	{
		return text;
	}
};