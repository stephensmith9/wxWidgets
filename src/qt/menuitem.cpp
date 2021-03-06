/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/menuitem.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/menuitem.h"
#include "wx/menu.h"
#include "wx/bitmap.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>

class wxQtAction : public QAction, public wxQtSignalHandler< wxMenuItem >
{

public:
    wxQtAction( wxMenu *parent, int id, const wxString &text, const wxString &help,
        wxItemKind kind, wxMenu *subMenu, wxMenuItem *handler );

private:
    void onActionTriggered( bool checked );
};


wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu, int id, const wxString& name,
    const wxString& help, wxItemKind kind, wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}



wxMenuItem::wxMenuItem(wxMenu *parentMenu, int id, const wxString& text,
        const wxString& help, wxItemKind kind, wxMenu *subMenu)
    : wxMenuItemBase( parentMenu, id, text, help, kind, subMenu )
{
    m_qtAction = new wxQtAction( parentMenu, id, text, help, kind, subMenu, this );
}



void wxMenuItem::SetItemLabel( const wxString &label )
{
    wxMenuItemBase::SetItemLabel( label );

    m_qtAction->setText( wxQtConvertString( label ));
}



void wxMenuItem::SetCheckable( bool checkable )
{
    wxMenuItemBase::SetCheckable( checkable );

    m_qtAction->setCheckable( checkable );
}



void wxMenuItem::Enable( bool enable )
{
    wxMenuItemBase::Enable( enable );

    m_qtAction->setEnabled( enable );
}



bool wxMenuItem::IsEnabled() const
{
    bool isEnabled = m_qtAction->isEnabled();

    // Make sure the enabled stati are in synch:
    wxASSERT( isEnabled == wxMenuItemBase::IsEnabled() );

    return isEnabled;
}



void wxMenuItem::Check( bool checked )
{
    wxMenuItemBase::Check( checked );

    m_qtAction->setChecked( checked );
}



bool wxMenuItem::IsChecked() const
{
    bool isChecked = m_qtAction->isChecked();

    // Make sure the checked stati are in synch:
    wxASSERT( isChecked == wxMenuItemBase::IsChecked() );

    return isChecked;
}


void wxMenuItem::SetBitmap(const wxBitmap& WXUNUSED(bitmap))
{
    wxMISSING_FUNCTION();
}

const wxBitmap &wxMenuItem::GetBitmap() const
{
    wxMISSING_FUNCTION();

    static wxBitmap s_bitmap;

    return s_bitmap;
}

QAction *wxMenuItem::GetHandle() const
{
    return m_qtAction;
}

//=============================================================================

wxQtAction::wxQtAction( wxMenu *parent, int id, const wxString &text, const wxString &help,
        wxItemKind kind, wxMenu *subMenu, wxMenuItem *handler )
    : QAction( wxQtConvertString( text ), parent->GetHandle() ),
      wxQtSignalHandler< wxMenuItem >( handler )
{
    setStatusTip( wxQtConvertString( help ));

    if ( subMenu != NULL )
        setMenu( subMenu->GetHandle() );

    if ( id == wxID_SEPARATOR )
        setSeparator( true );

    switch ( kind )
    {
        case wxITEM_SEPARATOR:
            setSeparator( true );
            break;
        case wxITEM_CHECK:
        case wxITEM_RADIO:
            setCheckable( true );
            break;
        case wxITEM_NORMAL:
            // Normal for a menu item.
            break;
        case wxITEM_DROPDOWN:
        case wxITEM_MAX:
            // Not applicable for menu items.
            break;
    }

    connect( this, &QAction::triggered, this, &wxQtAction::onActionTriggered );
}
 

void wxQtAction::onActionTriggered( bool checked )
{
    wxMenuItem *handler = GetHandler();
    wxMenu *menu = handler->GetMenu();
    menu->SendEvent( handler->GetId(), handler->IsCheckable() ? checked : -1 );
}
