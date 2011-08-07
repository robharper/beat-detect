// BDNodeCSN.h: interface for the CBDNodeCSN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BDNODECSN_H__5E0E5B73_4531_4FD5_86C2_EC57CDBA925F__INCLUDED_)
#define AFX_BDNODECSN_H__5E0E5B73_4531_4FD5_86C2_EC57CDBA925F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BDNode.h"



class CBDNodeCSN  
{
public:
	CBDNodeCSN( CBDNode * pNode );
	virtual ~CBDNodeCSN();

    HRESULT     Initialize();

    HRESULT     AddCSNLink( CBDNode * pNode );
    HRESULT     RemoveCSNLink( CBDNode * pNode );
    HRESULT     FlushCSNLinks();

    HRESULT     UpdateCSN( FLOAT flNetEnergy );
    HRESULT     CommitCSN();

    FLOAT       CSNOutput() const       { return m_flCSNOutput; }


protected:
    NodeList            m_lstLinks;

    FLOAT               m_flCSNOutput;
    FLOAT               m_flCSNOutputNew;

    CBDNode           * m_pNode;
};

#endif // !defined(AFX_BDNODECSN_H__5E0E5B73_4531_4FD5_86C2_EC57CDBA925F__INCLUDED_)
