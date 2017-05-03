/** \file
\brief Exception classes for error handling
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Error \n
Module components: \ref code_error
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_ERROR_H_
#define COMMON_ERROR_H_

typedef long HRESULT;
#ifdef _WIN32
	#include <windows.h>
#endif
#ifdef USE_DIRECTX
	struct ID3DXBuffer;
#endif

namespace common
{

/** \addtogroup code_error Error Module
Dokumentacja: \ref Module_Error \n
Nag��wek: Error.hpp */
//@{

/// Klasa bazowa b��d�w
class Error
{
private:
	std::vector<tstring> m_Msgs;

protected:
	/// U�yj tego konstruktora, je�li musisz najpierw co� policzy�, a dopiero potem utworzy� komunikat o b��dzie.
	Error() { }

public:
	/// Dodaje komunikat na stos.
	/** \param file u�yj __TFILE__
	\param line u�yj __LINE__ */
	void Push(const tstring &msg, const tstring &file = _T(""), int line = 0);
	/// Zwraca pe�ny opis b��du w jednym �a�cuchu
	/** \param Indent �a�cuch od kt�rego ma si� zaczyna� ka�dy komunikat (wci�cie).
	\param EOL �a�cuch oddzielaj�cy komunikaty (koniec wiersza). Nie do��czany na ko�cu. */
	void GetMessage_(tstring *Msg, const tstring &Indent = _T(""), const tstring &eol = EOL) const;

	/// Utworzenie og�lnego b��du
	Error(const tstring &msg, const tstring &file = _T(""), int line = 0)
	{
		Push(msg, file, line);
	}
};

/// Wyj�tek utworzony z systemowego b��du errno
class ErrnoError : public Error
{
public:
	/// Tworzy b��d z podanym kodem.
	ErrnoError(int err_code, const tstring &msg, const tstring &file = _T(""), int line = 0);
	/// Tworzy b��d z kodem pobieranym ze zmiennej errno.
	ErrnoError(const tstring &msg, const tstring &file = _T(""), int line = 0);
};

#ifdef USE_SDL
	/// Wyj�tek z b��dem pobranym z biblioteki SDL za pomoc� SDLError
	class SDLError : public Error
	{
	public:
		SDLError(const tstring &msg, const tstring &file = _T(""), int line = 0);
	};
#endif

#ifdef USE_OPENGL
	/// Wyj�tek z b��dem pobranym z biblioteki OpenGL za pomoc� OpenGLError
	class OpenGLError : public Error
	{
	public:
		OpenGLError(const tstring &msg, const tstring &file = _T(""), int line = 0);
	};
#endif

#ifdef USE_FMOD
	/// Wyj�tek z b��dem pobranym z biblioteki d�wi�kowej FMOD
	class FmodError : public Error
	{
	private:
		/** \param[out] Enum Mo�e by� NULL.
		\param[out] Message Mo�e by� NULL. */
		void CodeToMessage(FMOD_RESULT code, tstring *Enum, tstring *Message);

	public:
		FmodError(FMOD_RESULT code, const tstring &msg, const tstring &file = _T(""), int line = 0);
	};
#endif

#ifdef _WIN32
	/// Tworzy wyj�tek na podstawie b��du WinAPI pobieranego za pomoc� GetLastError
	class Win32Error : public Error
	{
	public:
		Win32Error(const tstring &msg = _T(""), const tstring &file = _T(""), int line = 0);
	};
#endif

#ifdef USE_DIRECTX
	/// Tworzy wyj�tek na podstawie b��du biblioteki Direct3D.
	class DirectXError : public Error
	{
	public:
		DirectXError(HRESULT hr, const tstring &Msg = _T(""), const tstring &file = _T(""), int Line = 0);
		/// U�yj tej wersji je�li funkcja DX kt�r� kontroluje zwraca te� w przypadku b��du bufor z komunikatem
		/** Bufor zostanie przez konstruktor tego wyj�tku sam zwolniony, je�li istnia�. Mo�e nie istnie�. */
		DirectXError(HRESULT hr, ID3DXBuffer *Buf, const tstring &Msg = _T(""), const tstring &file = _T(""), int Line = 0);
	};
#endif

#ifdef USE_WINSOCK
	/// Tworzy wyj�tek na podstawie b��du biblioteki WinSock.
	class WinSockError : public Error
	{
	private:
		void CodeToStr(int Code, tstring *Name, tstring *Message);

	public:
		/// Tworzy komunikat b��du WinSockError
		int WinSockError(int Code, const tstring &Msg = "", const tstring &file = _T(""), int Line = 0);
		/// Tworzy komunikat b��du WinSockError, pobiera kod sam z WSAGetLastError()
		int WinSockError(const tstring &Msg = "", const tstring &file = _T(""), int Line = 0);
	};
#endif

#ifdef USE_DEVIL
	/// Tworzy wyj�tek na podstawie b��du biblioteki graficznej DevIL
	/* Poniewa� nie spos�b niestety w DevIL sprawdzi� czy jest b��d bez zdj�cia jednego
	kodu b��du ze szczytu stosu, nale�y wywo�a� ILenum code = ilGetError() i je�li
	code != IL_NO_ERROR, to wtedy rzuci� ten wyj�tek podaj�c jako pierwszy parametr code.
	*/
	class DevILError : public Error
	{
	public:
		DevILError(unsigned FirstCode, const tstring &Msg = "", const tstring &File = _T(""), int Line = 0);
	};
#endif

#ifdef USE_AVI_FILE
	/// Tworzy wyj�tek na podstawie b��du biblioteki AviFile (cz�� WinAPI)
	class AVIFileError : public Error
	{
	public:
		AVIFileError(HRESULT hr, const tstring &Msg = "", const tstring &file = _T(""), int a_Line = 0);
	};
#endif

//@}
// code_error

} // namespace common

/** \addtogroup code_error */
//@{

/// U�yj aby doda� komunikat do stosu wyj�tku
#define ERR_ADD_FUNC(exception) { (exception).Push(__TFUNCSIG__, __TFILE__, __LINE__); }
#define ERR_TRY        { try {
#define ERR_CATCH(msg) } catch(common::Error &e) { e.Push((msg), __TFILE__, __LINE__); throw; } }
#define ERR_CATCH_FUNC } catch(common::Error &e) { ERR_ADD_FUNC(e); throw; } }

#define ERR_GUARD_BOOL(Expr) { if ((Expr) == false) throw common::Error(_T(#Expr), __TFILE__, __LINE__); }
#define ERR_GUARD_DIRECTX(Expr) { HRESULT hr; if (FAILED(hr = (Expr))) throw common::DirectXError(hr, _T(#Expr), __TFILE__, __LINE__); }
#define ERR_GUARD_DEVIL(Code) { { Code ; } ILenum c = ilGetError(); if (c != IL_NO_ERROR) throw common::DevILError(c, _T(#Code), __TFILE__, __LINE__); }

#ifdef _DEBUG
	#define ERR_GUARD_BOOL_D(Expr) ERR_GUARD_BOOL(Expr)
	#define ERR_GUARD_DIRECTX_D(Expr) ERR_GUARD_DIRECTX(Expr)
#else
	#define ERR_GUARD_BOOL_D(Expr) (Expr);
	#define ERR_GUARD_DIRECTX_D(Expr) (Expr);
#endif

/// Makra, kt�re przerabiaj� ka�dy rodzaj wyj�tku na wyj�tek common::Error
#define ALL_EXCEPTIONS_TRY   try {
/// Makra, kt�re przerabiaj� ka�dy rodzaj wyj�tku na wyj�tek common::Error
#define ALL_EXCEPTIONS_CATCH   \
	} \
	catch (const common::Error&) \
	{ \
		throw; \
	} \
	catch (const std::exception &stde) \
	{ \
		throw common::Error(tstring(_T("std::exception: ")) + common::StringToTstringR(stde.what()), __TFILE__, __LINE__); \
	} \
	catch (...) \
	{ \
		throw common::Error(_T("Unknown exception."), __TFILE__, __LINE__); \
	}

//@}
// code_error

/// Rozszerzenie modu�u bazowego o wsparcie dla b��d�w
/**
\ingroup code_sthtostr
\ingroup code_error */
template <typename T>
inline void MustStrToSth(T *Sth, const tstring &Str)
{
	if (!StrToSth<T>(Sth, Str))
		throw common::Error(_T("String conversion error: ") + Str);
}

#endif
