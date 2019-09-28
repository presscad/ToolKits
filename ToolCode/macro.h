#ifndef __LDS_PLATFORM_MACRO_HEADER_
#define __LDS_PLATFORM_MACRO_HEADER_
//���ļ����ڹ�������LDSƽ̨������������Խ��Խ��Ĳ�Ʒ�������ĸ��Ӻ궨��
#if defined(__LDS_)||defined(__TMA_)||defined(__TSA_)||defined(__LMA_)||defined(__TDA_)
	#ifndef __LDS_CONTEXT_
		#define __LDS_CONTEXT_
	#endif
#elif defined(__LDSGROUP_)||defined(__TMAGROUP_)||defined(__TSAGROUP_)||defined(__LMAGROUP_)||defined(__TDAGROUP_)
	#ifndef __LDS_GROUP_CONTEXT_
		#define __LDS_GROUP_CONTEXT_
	#endif
#elif defined(__MAP_LDS_)||defined(__MAP_TMA_)||defined(__MAP_TSA_)||defined(__MAP_LMA_)||defined(__MAP_TDA_)
	#ifndef __DRAWING_CONTEXT_
		#define __DRAWING_CONTEXT_
	#endif
#endif
#if defined(__LDS_)||defined(__LDSGROUP_)||defined(__MAP_LDS_)	//��Ʒ��ģ�����ƺ궨��
	#ifndef __LDS_FILE_
		#define __LDS_FILE_	//ģ�����ݶ���
		//#define __GGG_
	#endif
#elif defined(__TMA_)||defined(__TMAGROUP_)||defined(__MAP_TMA_)
	#ifndef __TMA_FILE_
		#define __TMA_FILE_
	#endif
#elif defined(__LMA_)||defined(__LMAGROUP_)||defined(__MAP_LMA_)
	#ifndef __LMA_FILE_
		#define __LMA_FILE_
	#endif
#elif defined(__TSA_)||defined(__TSAGROUP_)||defined(__MAP_TSA_)
	#ifndef __TSA_FILE_
		#define __TSA_FILE_
	#endif
#elif defined(__TDA_)||defined(__TDAGROUP_)//||defined(__MAP_TDA_)
	#ifndef __TDA_FILE_
		#define __TDA_FILE_
	#endif
#endif
//����ģ�����ݺ궨��ȷ�����幹�������������궨��
#if defined(__LDS_FILE_)||defined(__TSA_FILE_)
	#ifndef __FEM_ANALYZE_INC_
		#define __FEM_ANALYZE_INC_	//��������Ԫ��ѧ�������㹦��
	#endif
#endif
#if defined(__LDS_FILE_)||defined(__TSA_FILE_)
	#ifndef __PART_DESIGN_INC_
		#define __PART_DESIGN_INC_	//����������ƹ��ܣ�������֪�������е�Ԫ���㣬�Ż�ѡ�ģ�
	#endif
#endif
#if defined(__LDS_FILE_)||defined(__TMA_FILE_)||defined(__TDA_FILE_)||defined(__LMA_FILE_)
	#ifndef __ANGLE_PART_INC_
		#define __ANGLE_PART_INC_	//�Ǹֹ�������
	#endif
#endif
#if defined(__LDS_FILE_)||defined(__TDA_FILE_)||defined(__LMA_FILE_)
	#ifndef __COMPLEX_PART_INC_
		#define __COMPLEX_PART_INC_	//�Ǹ���ȸ��ิ���͸ֹ�������
	#endif
#endif
#if defined(__ANGLE_PART_INC_)||defined(__COMPLEX_PART_INC_)
	#ifndef __COMMON_PART_INC_
		#define __COMMON_PART_INC_	//����ֹ�����ְ塢��˨��
	#endif
#endif
#if defined(__LDS_FILE_)||defined(__TDA_FILE_)
	#ifndef __SMART_DRAW_INC_
		#define __SMART_DRAW_INC_
	#endif
#endif

#if defined(__TMA_FILE_)
#define PRODUCT_TYPE 1
#elif defined(__LMA_FILE_)
#define PRODUCT_TYPE 2
#elif defined(__TSA_FILE_)
#define PRODUCT_TYPE 3
#elif defined(__LDS_FILE_)
#define PRODUCT_TYPE 4
#else
#define PRODUCT_TYPE 5
#endif

#endif
