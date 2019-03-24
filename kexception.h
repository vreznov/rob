#ifndef EXCEPTIONBASE_H
#define EXCEPTIONBASE_H

/*  文件名：kexception.h
 *  日期：2019.02.04
 *  作者： Kare
 *  版本：1.0
 *  功能：提供错误基类
 * */

#include <string>
#include <exception>
using namespace std;

class KException : public std::exception
{
public:
    KException();

    //!
    //! @param fml_sInfo: error info
    //! @param fml_errId: error id
    KException(const string& fml_sInfo, int fml_errId = 0);

    /*!
     \return the error description and the context as a text string.
    */
//    virtual const char *what() const _GLIBCXX_USE_NOEXCEPT;
    virtual const char *what() const _GLIBCXX_USE_NOEXCEPT override
    {
        return msg.c_str();
    }

	string msg; ///< the formatted error message

	int code; ///< error code @see CVStatus
	string err; ///< error description
	string func; ///< function name. Available only when the compiler supports getting it
	string file; ///< source file name where the error has occured
	int line; ///< line number in the source file where the error has occured
};

#endif // EXCEPTIONBASE_H
