/*
Launchy: Application Launcher
Copyright (C) 2026  Caelan Borowiec

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "precompiled.h"
#include "storeapps.h"

static const QString appsFolderPrefix = "shell:AppsFolder\\";


storeappsPlugin::storeappsPlugin()
{
	HASH_storeapps = qHash(QString("storeapps"));
}

storeappsPlugin::~storeappsPlugin()
{
}


void storeappsPlugin::getID(uint* id)
{
	*id = HASH_storeapps;
}

void storeappsPlugin::getName(QString* str)
{
	*str = "Store Apps";
}


static QString shellItemName(IShellItem* item, SIGDN sigdn)
{
	QString name;
	PWSTR buffer = NULL;
	if (SUCCEEDED(item->GetDisplayName(sigdn, &buffer)))
	{
		name = QString::fromWCharArray(buffer);
		CoTaskMemFree(buffer);
	}
	return name;
}


void storeappsPlugin::getCatalog(QList<CatItem>* items)
{
	// The catalog is built on a worker thread; RPC_E_CHANGED_MODE still leaves COM usable
	HRESULT hrInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	IShellItem* appsFolder = NULL;
	if (SUCCEEDED(SHGetKnownFolderItem(FOLDERID_AppsFolder, KF_FLAG_DEFAULT, NULL, IID_PPV_ARGS(&appsFolder))))
	{
		IEnumShellItems* children = NULL;
		if (SUCCEEDED(appsFolder->BindToHandler(NULL, BHID_EnumItems, IID_PPV_ARGS(&children))))
		{
			IShellItem* child = NULL;
			while (children->Next(1, &child, NULL) == S_OK)
			{
				// Packaged apps are "PackageFamilyName!AppId"; anything else is a desktop
				// app that the Start Menu shortcuts already cover
				QString aumid = shellItemName(child, SIGDN_PARENTRELATIVEPARSING);
				if (aumid.contains('!'))
				{
					QString name = shellItemName(child, SIGDN_NORMALDISPLAY);
					if (!name.isEmpty())
						items->push_back(CatItem(appsFolderPrefix + aumid, name, HASH_storeapps));
				}
				child->Release();
			}
			children->Release();
		}
		appsFolder->Release();
	}

	if (SUCCEEDED(hrInit))
		CoUninitialize();
}


int storeappsPlugin::launchItem(QList<InputData>* inputData, CatItem* item)
{
	Q_UNUSED(inputData);

	if (!item->fullPath.startsWith(appsFolderPrefix, Qt::CaseInsensitive))
		return 1;

	HRESULT hrInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	SHELLEXECUTEINFOW sei;
	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.fMask = SEE_MASK_FLAG_NO_UI;
	sei.lpFile = reinterpret_cast<LPCWSTR>(item->fullPath.utf16());
	sei.nShow = SW_SHOWNORMAL;

	if (!ShellExecuteExW(&sei))
	{
		QString aumid = item->fullPath.mid(appsFolderPrefix.length());
		IApplicationActivationManager* activationManager = NULL;
		if (SUCCEEDED(CoCreateInstance(CLSID_ApplicationActivationManager, NULL, CLSCTX_LOCAL_SERVER,
									   IID_PPV_ARGS(&activationManager))))
		{
			DWORD processId = 0;
			activationManager->ActivateApplication(reinterpret_cast<LPCWSTR>(aumid.utf16()), NULL, AO_NONE, &processId);
			activationManager->Release();
		}
	}

	if (SUCCEEDED(hrInit))
		CoUninitialize();

	return 1;
}


int storeappsPlugin::msg(int msgId, void* wParam, void* lParam)
{
	int handled = 0;
	switch (msgId)
	{
		case MSG_INIT:
			handled = 1;
			break;
		case MSG_GET_ID:
			getID((uint*) wParam);
			handled = 1;
			break;
		case MSG_GET_NAME:
			getName((QString*) wParam);
			handled = 1;
			break;
		case MSG_GET_CATALOG:
			getCatalog((QList<CatItem>*) wParam);
			handled = 1;
			break;
		case MSG_LAUNCH_ITEM:
			handled = launchItem((QList<InputData>*) wParam, (CatItem*) lParam);
			break;

		default:
			break;
	}

	return handled;
}
