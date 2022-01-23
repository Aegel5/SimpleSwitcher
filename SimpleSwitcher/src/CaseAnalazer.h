#pragma once

struct CaseAnalazer
{
	enum ECase
	{
		CA_CASE_NONE,

		CA_CASE_MIX,
		CA_CASE_UPPER,
		CA_CASE_LOWER,
	};

	ECase m_curCase;

	ECase m_origCase;
	//std::vector<CHotKey> m_origKeys;
	tstring m_origText;

	void GenNextState()
	{
		if (m_curCase == CA_CASE_NONE)
		{
			m_curCase = m_origCase;
		}

		if (m_curCase == CA_CASE_MIX)
		{
			m_curCase = CA_CASE_UPPER;
		}
		else if (m_curCase == CA_CASE_UPPER)
		{
			m_curCase = CA_CASE_LOWER;
		}
		else if (m_curCase == CA_CASE_LOWER)
		{
			//m_curCase = m_origCase == CA_CASE_MIX ? CA_CASE_MIX : CA_CASE_UPPER;
			m_curCase = CA_CASE_UPPER;
		}
		else
		{
			IFS_LOG(SW_ERR_BAD_INTERNAL_STATE);
		}
	}

	void GenerateNexCurKeys(tstring& txt)
	{
		GenNextState();
		txt = m_origText;

		if (m_curCase == CA_CASE_UPPER)
		{
			Str_Utils::ToUpper(txt);
		}
		else if (m_curCase == CA_CASE_LOWER)
		{
			Str_Utils::ToLower(txt);
		}
	}

	void AddOrigText(tstring& txt)
	{
		Clear();
		m_origText = txt;
		m_origCase = GetCase(txt);
	}

	bool IsInited()
	{
		return m_origCase != CA_CASE_NONE;
	}

	void Clear()
	{
		LOG_INFO_4(L"CaseAnalazer clear");

		m_curCase = CA_CASE_NONE;
		m_origCase = CA_CASE_NONE;
		m_origText.clear();
	}

	ECase GetCase(tstring& txt)
	{
		if (txt.empty())
		{
			return CA_CASE_LOWER;
		}

		bool fLowFound = false;
		bool fUpFound = false;
		for (auto k : txt)
		{
			if (std::isupper(k))
			{
				fUpFound = true;
			}
			else
			{
				fLowFound = true;
			}
			if (fUpFound && fLowFound)
			{
				return CA_CASE_MIX;
			}
		}
		if (fUpFound)
		{
			return CA_CASE_UPPER;
		}
		else
		{
			return CA_CASE_LOWER;
		}
	}
};