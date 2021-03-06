/* dialogs.cpp - Dialog procedures for ODMA sample DMS.
 *
 * COPYRIGHT (C) 1994, 1995
 * AIIM International
 * All Rights Reserved
 */

#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include "odmasamp.h"
#include <strsafe.h>

BOOL CALLBACK _export SelectDocProc(HWND hwndDlg, UINT message, WPARAM wParam,
LPARAM lParam)
{
static char *DocId;

	switch(message) {
		case WM_INITDIALOG:
		{
			DocId = reinterpret_cast<char *>(lParam);
            int count = 0;
		    for(int ii = 0; ii<MAXDOCS; ii++) 
			{
				Document *pDoc = DocList.GetDocumentByIndex(ii);

				if(pDoc == nullptr)
					continue;

				//BVG: Set Doc Name into List Box 
				LPSTR lpNowDocName=new char[DOC_NAME_MAX];
				pDoc->GetInfo(ODM_NAME, lpNowDocName, DOC_NAME_MAX);
			    auto index=static_cast<size_t>(SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(lpNowDocName))
			    );
				delete[] lpNowDocName;

				//BVG: Save DocId as ItemData
				LPSTR lpNowDocId = new char[ODM_DOCID_MAX];
				StringCchCopy(lpNowDocId, ODM_DOCID_MAX, pDoc->GetId());
				SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_SETITEMDATA, index, reinterpret_cast<LPARAM>(lpNowDocId));

				count++;
			}

			if(count == 0) 
			{
				// No documents in the DMS!
				EnableWindow(GetDlgItem(hwndDlg, IDOK), FALSE );
			}
			else 
			{
				// Select the first item in the list.
				SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_SETCURSEL, 0, 0);
			}

			return TRUE;
		}

		case WM_COMMAND: 
			switch(GET_WM_COMMAND_ID(wParam, lParam)) 
			{
				case IDC_SELECT:
				{
					if(GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK) 
					{
						SendMessage(hwndDlg, WM_COMMAND, IDOK, lParam);
					}
					break;
				}

				case IDOK:
				{	
					//BVG: Get DocId from List Boxe's ItemData
					auto index = static_cast<size_t>(SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_GETCURSEL, 0, 0));
					StringCchPrintf(DocId, ODM_DOCID_MAX, "::ODMA\\%s\\%s", DMSID,
						reinterpret_cast<LPSTR>(SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_GETITEMDATA, index, 0)) );
				}


				//Fall through.
				case IDCANCEL:
				case IDAPPSELECT:
				{
				    //BVG: Memory Clean Up
                    const auto size = static_cast<size_t>(SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_GETCOUNT, 0, 0));
					for(size_t ii = 0; ii < size; ii++)
					{
						LPSTR lpNowDocId = reinterpret_cast<LPSTR>(SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_GETITEMDATA, ii, 0));
						delete[] lpNowDocId;
					}
					EndDialog(hwndDlg, static_cast<INT_PTR>(wParam));
					return TRUE;
				}
			}
	}
	return FALSE;
}

BOOL CALLBACK _export SelectDocProcEx(HWND hwndDlg, UINT message, WPARAM wParam,
LPARAM lParam)
{
static char *lpDocList;
	//TODO check ownership rules for LB_GETDATA and LB_SETDATA

	switch(message) {
		case WM_INITDIALOG:
		{
			lpDocList = reinterpret_cast<char *>(lParam);
		    int count = 0;
			for(int ii=0; ii<MAXDOCS; ii++) 
			{
				Document *pDoc = DocList.GetDocumentByIndex(ii);

				if(pDoc == nullptr)
					continue;

				//BVG: Set Doc Name into List Box 
				LPSTR lpNowDocName=new char[DOC_NAME_MAX];
				pDoc->GetInfo(ODM_NAME, lpNowDocName, DOC_NAME_MAX);
				const auto index = (size_t)SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(lpNowDocName));
				delete[] lpNowDocName;

				//BVG: Save DocId as ItemData
				LPSTR lpNowDocId = new char[ODM_DOCID_MAX];
				StringCchCopy(lpNowDocId, ODM_DOCID_MAX, pDoc->GetId());
				SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_SETITEMDATA, index, reinterpret_cast<LPARAM>(lpNowDocId));

				count++;
			}

			return TRUE;
		}

		case WM_ACTIVATE:
			return TRUE;


		case WM_COMMAND: 
			switch(GET_WM_COMMAND_ID(wParam, lParam)) 
			{
				case IDC_SELECT:
					if(GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK) 
					{
						SendMessage(hwndDlg, WM_COMMAND, IDOK, lParam);
					}
					break;

				case IDOK:
				{
					//Get DocIds from List Boxe's ItemData
					if (SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_GETSELCOUNT, 0, 0) == 0) break;

				    const auto count = static_cast<size_t>(SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_GETCOUNT, 0, 0));

					strcpy(lpDocList, "");
					char	szDocId[ODM_DOCID_MAX];

					int end = 0;
					for (size_t ii = 0; ii < count; ii++)
					{
						if (SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_GETSEL, ii, 0))
						{
							sprintf_s(szDocId, "::ODMA\\%s\\%s", DMSID,
								reinterpret_cast<LPSTR>(SendDlgItemMessage(hwndDlg, IDC_SELECT,
									LB_GETITEMDATA, ii, 0)));

							strcpy(lpDocList + end, &szDocId[0]);
							end += strlen(&szDocId[0]) + 1;
						}
					}
					strcpy(lpDocList + end, "");
				}


				//Fall through.
				case IDCANCEL:
				case IDAPPSELECT:
				{
				    //BVG: Memory Clean Up
					for(int ii=0; ii<SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_GETCOUNT, 0, 0); ii++)
					{
						LPSTR lpNowDocId = reinterpret_cast<LPSTR>(SendDlgItemMessage(hwndDlg, IDC_SELECT, LB_GETITEMDATA, ii, 0));
						delete [] lpNowDocId;
					}
					EndDialog(hwndDlg, wParam);
					return TRUE;
				}
			}
	}
	return FALSE;
}


BOOL CALLBACK _export SaveAsProc(HWND hwndDlg, UINT message, WPARAM wParam,
LPARAM lParam)
{
static SaveAsData *pSaveAsData;
char *lp;

	switch(message) {
		case WM_INITDIALOG:
			pSaveAsData = reinterpret_cast<SaveAsData *>(lParam);
			SendDlgItemMessage(hwndDlg, ST_FORMAT, WM_SETTEXT, 0,
					reinterpret_cast<LPARAM>(pSaveAsData->Format));

			if(pSaveAsData->pcbCallBack == nullptr)
				ShowWindow(GetDlgItem(hwndDlg, IDC_OPTIONS), SW_HIDE);

			return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_OPTIONS:
					lp = pSaveAsData->pcbCallBack((DWORD_PTR)hwndDlg, pSaveAsData->Format,
						pSaveAsData->pInstanceData);

					strcpy_s(pSaveAsData->Format, lp);
					SendDlgItemMessage(hwndDlg, ST_FORMAT, WM_SETTEXT, 0,
						reinterpret_cast<LPARAM>(pSaveAsData->Format));

					return TRUE;

				case IDOK:
				case IDCOPY:
				case IDCANCEL:
					EndDialog(hwndDlg, wParam);
					return TRUE;
			}
	}
	return FALSE;
}


BOOL CALLBACK _export ProfileProc( HWND hwndDlg, UINT message, WPARAM wParam,
LPARAM lParam)
{
static ProDlgData *pDlgData;

	switch(message) {
		case WM_INITDIALOG:
			pDlgData = reinterpret_cast<ProDlgData *>(lParam);
			SendDlgItemMessage(hwndDlg, ST_DOCID, WM_SETTEXT, 0,
				reinterpret_cast<LPARAM>(pDlgData->pDocument->GetId()));

			SendDlgItemMessage(hwndDlg, IDC_NAME, WM_SETTEXT, 0,
				reinterpret_cast<LPARAM>(pDlgData->pDocument->Name));

			SendDlgItemMessage(hwndDlg, IDC_AUTHOR, WM_SETTEXT, 0,
				reinterpret_cast<LPARAM>(pDlgData->pDocument->Author));

			SendDlgItemMessage(hwndDlg, IDC_TYPE, WM_SETTEXT, 0,
				reinterpret_cast<LPARAM>(pDlgData->pDocument->DocType));

			if(pDlgData->Mode & PROFILE_EDIT) 
			{
				SendDlgItemMessage(hwndDlg, IDC_NAME, EM_SETREADONLY, 0, 0);
				SendDlgItemMessage(hwndDlg, IDC_AUTHOR, EM_SETREADONLY, 0, 0);
				SendDlgItemMessage(hwndDlg, IDC_TYPE, EM_SETREADONLY, 0, 0);
				SetFocus(GetDlgItem(hwndDlg, IDC_NAME ));
			} else {
				SetFocus(GetDlgItem(hwndDlg, IDOK));
				ShowWindow(GetDlgItem(hwndDlg, IDCANCEL), SW_HIDE);
			}
			return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDOK:
					if(pDlgData->Mode & PROFILE_EDIT) {
						SendDlgItemMessage(hwndDlg, IDC_NAME, WM_GETTEXT,
							_countof(pDlgData->pDocument->Name),
							reinterpret_cast<LPARAM>(pDlgData->pDocument->Name));

						SendDlgItemMessage(hwndDlg, IDC_AUTHOR, WM_GETTEXT,
							_countof(pDlgData->pDocument->Author),
							reinterpret_cast<LPARAM>(pDlgData->pDocument->Author));

						SendDlgItemMessage(hwndDlg, IDC_TYPE, WM_GETTEXT,
							_countof(pDlgData->pDocument->DocType),
							reinterpret_cast<LPARAM>(pDlgData->pDocument->DocType));
					}

					// fall through to below

				case IDCANCEL:
					EndDialog(hwndDlg, wParam);
					return TRUE;
			}
	}
	return FALSE;
}

