// UGupdate.cpp: �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <uf.h>
#include <uf_modl.h>
#include <uf_ps.h>
#include <uf_obj.h>
#include <uf_assem.h>
#include <uf_part.h>
#include <uf_modl_expressions.h>
using namespace std;
/*
����:
��exp�ļ�����prt�ļ��ģ�����prt�ļ����Ϊx_t�ļ�
���÷���:
system("you.prt   you.exp   you.x_t")
createprocess("you.prt"��"you.exp   d:\\you.x_t"...)
ע�⣺
�ļ���һ��Ҫдȫ���ڵ���ʱ���Զ�ɾ���ɵ��ļ��������
��Ϊ�ļ��Ѿ����ڶ����µ���x_t�ļ�ʧ�ܡ�

san,nuaa,202
����������������������������������������visualsan@yahoo.cn
					2011.11.13
					*/
int  prt_save_as_xt(tag_t, string ps);
//����lib�ļ������˶���
#pragma  comment(lib,"C:\\Program Files\\Siemens\\NX 11.0\\UGOPEN\\libufun.lib")
#define VISUALSAN_NUAA_202_RUN(x)\
if(0!=x)\
{\
    char msg[133];\
    UF_get_fail_message( x,msg );\
    cout<<msg;\
    return -1;\
}

int  main(int argc, char **argv)
{

	//ʵ�ֳ�ʼ��ug api,��������ug�������޷�ʹ�õ�
	VISUALSAN_NUAA_202_RUN(UF_initialize());

	//�ļ�·��
	string  prt, ep;
	string  x_t;//x_t��ʽ�ļ���ַ

				/*����������飬
				ע����system("UG_update.exe xx.prt  yy.exp")����ʱ��
				Ĭ�ϵ�һ������Ϊexe��ַ��������Ĳ���
				�ǵڶ�����ʼ����createprocessʱ����������㴫�ݵ�һ��
				system("UG_update.exe xx.prt  yy.x_t")
				argv[0]=UG_update.exe argv[1]=xx.prt argv[2]=yy.exp  argv[3]=yy.x_t

				createprocess("UG_update.exe","xx.prt  yy.exp yy.x_t",....)
				argv[0]=xx.prt argv[1]=yy.exp argv[2]=yy.x_t*/

	if (argc<3)
	{
		cerr << "������������";
		return  -1;
	}
	//Ĭ��������createprocess�����̵߳�
	if (argc == 3)
	{
		prt = argv[0];  //prt�ļ�
		ep = argv[1];  //exp�ļ�
		x_t = argv[2]; //x_t�ļ�
	}
	//��������system���õ�
	else
	{
		prt = argv[1]; //prt�ļ�
		ep = argv[2]; //exp�ļ�
		x_t = argv[3]; //x_t�ļ�
	}

	//��ģ���ļ�
	UF_PART_load_status_t st;
	tag_t  prt_id;
	//��prt�ļ�
	VISUALSAN_NUAA_202_RUN(UF_PART_open(prt.c_str(), &prt_id, &st));
	//����ģ���ļ�
	VISUALSAN_NUAA_202_RUN(UF_MODL_import_exp((char*)ep.c_str(), 0));
	//����ģ��
	VISUALSAN_NUAA_202_RUN(UF_MODL_update());
	//д���ļ�
	VISUALSAN_NUAA_202_RUN(UF_PART_save());

	//���Ϊx_t�ļ�
	if (-1 == prt_save_as_xt(prt_id, x_t))
		return -1;

	//�ر�prt�ļ�
	VISUALSAN_NUAA_202_RUN(UF_PART_close(prt_id, 1, 1));
	VISUALSAN_NUAA_202_RUN(UF_PART_free_load_status(&st));
	//�˳�ǰUF_terminate��������
	VISUALSAN_NUAA_202_RUN(UF_terminate());

	return 0;
}
int  prt_save_as_xt(tag_t body_tag, string ps)
{
	//û�к�׺��ʱ�����Ϻ�׺��
	if (std::string::npos == ps.find(".x_t"))
		ps += ".x_t";
	//���ô���:Ug2Ansys.cpp  �� ��, �й����ʴ�ѧ(����) 2006.12.31
	uf_list_p_t body_list;
	// ���װ����������root_part_occ, ����������NULL_TAGʱ, ������ǰ�����ļ���û��װ��(����������)
	tag_t root_part_occ = UF_ASSEM_ask_root_part_occ(body_tag);
	VISUALSAN_NUAA_202_RUN(UF_MODL_create_list(&body_list));
	// ����ǵ�������
	if (root_part_occ == NULL_TAG)
	{
		tag_t object = NULL_TAG;
		int UF_body_type;
		int type;
		int subtype;
		do {
			VISUALSAN_NUAA_202_RUN(
				UF_OBJ_cycle_objs_in_part(body_tag, UF_solid_type, &object)
			);
			if (object != NULL_TAG)
			{
				VISUALSAN_NUAA_202_RUN(
					UF_OBJ_ask_type_and_subtype(object, &type, &subtype)
				);
				VISUALSAN_NUAA_202_RUN(
					UF_MODL_ask_body_type(object, &UF_body_type)
				);

				if (subtype != UF_solid_body_subtype)
					continue;
				if (UF_body_type == UF_MODL_SOLID_BODY)
				{
					VISUALSAN_NUAA_202_RUN(
						UF_MODL_put_list_item(body_list, object)
					);
					break;
				}
			}
		} while (1);

	}
	//�����װ����
	else
	{
		tag_t obj = UF_ASSEM_ask_prototype_of_occ(root_part_occ);
		tag_t object = NULL_TAG;
		int UF_body_type;
		int type;
		int subtype;
		do
		{
			VISUALSAN_NUAA_202_RUN(
				UF_OBJ_cycle_objs_in_part(body_tag, UF_solid_type, &object));
			if (object != NULL_TAG)
			{
				VISUALSAN_NUAA_202_RUN(
					UF_OBJ_ask_type_and_subtype(object, &type, &subtype));
				// �ж�body�Ƿ���һ��Solid��Sheet
				VISUALSAN_NUAA_202_RUN(
					UF_MODL_ask_body_type(object, &UF_body_type));

				if (subtype != UF_solid_body_subtype)
					continue;
				if (UF_body_type == UF_MODL_SOLID_BODY)
				{
					// ��������뵽�����β��
					VISUALSAN_NUAA_202_RUN(
						UF_MODL_put_list_item(body_list, object));
				}
			}
			else
			{
				break;
			}

		} while (1);

	}

	// ����ļ�����, ��ɾ��
	remove(ps.c_str());
	// ����Parasolid�ļ�
	VISUALSAN_NUAA_202_RUN(
		UF_PS_export_data(body_list, (char*)ps.c_str()));
	// ɾ������
	VISUALSAN_NUAA_202_RUN(
		UF_MODL_delete_list(&body_list));

	return 0;
}

