#ifndef SELFCOLLISION
#define SELFCOLLISION

#include "cmesh.h"
#include "tmbvh.hpp"
#include "CollisionDate.h"

#include "iostream"
#include<vector>

using namespace std;

typedef pair<int, int> mesh_pair;

class Collision{
public:

	~Collision(){
		for (int i = 0; i < dl_mesh.size(); i++) {
				delete dl_mesh[i];
			}
	}

	//���ú����ӿڣ������ڲ���ײ����㷨
	void Collid();

	//��ײ������ӿ�
	void Transform_Pair(unsigned int a,unsigned int b);
	
	//ģ����������ӿڣ�����ģ������ĵ㼯���漯
	void Transform_Mesh(unsigned int numVtx, unsigned int numTri, const vector<unsigned int>& tris, 
		const vector<double>& vtxs,
		const vector<double>& pre_vtxs,
		int m_id,bool able_selfcollision=false
		);

	//����ӿڣ����ط�����ײ��ģ���������������Ƭ�ļ���
	vector<vector<tri_pair>> getContactPairs(){ return contact_pairs; }

	//����ӿڣ����ط�����ײ����ײ������
	int getNumContacts(){ return contact_pairs.size(); }
	
	//����ӿڣ�������ײ�Է�����ײ��ʱ��
	vector<double> getContactTimes() { return contact_time; }

	static Collision* getInstance()			
	{
		if (instance == NULL) {
			instance = new Collision();
			return instance;
		}
		else
			return instance;
	}


	static Collision* instance;
  	Collision():_is_first(true){}

private:
	vector<CollisionDate> bodys;     
	vector<mesh_pair> mesh_pairs;
	vector<vector<tri_pair>> contact_pairs;
	vector<mesh*> dl_mesh;//delete mesh points
	vector<double> contact_time;



	bool _is_first;//�Ƿ��һ�δ�������

};


#endif
