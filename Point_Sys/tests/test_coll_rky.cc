#include <iostream>
#include <limits>
#include <Collision/CollisionDetect-cloth/src/Collision.h>
using namespace std;

const double DOUBLE_MAX = std::numeric_limits<double>::max();

int main(int argc, char** argv){
  cout << "[INFO]>>>>>>>>>>>>>>>>>>>SET CUBE<<<<<<<<<<<<<<<<<<" << endl;
  vector<unsigned int> cube_surf =
      {1,3,0,
       7,5,4,
       4,1,0,
       5,2,1,
       2,7,3,
       0,7,4,
       1,2,3,
       7,6,5,
       4,5,1,
       5,6,2,
       2,6,7,
       0,3,7};
  
  vector<double> cube_nods =
      {1,-1,-1,
       1,-1,1,
       -1,-1,1,
       -1,-1,-1,
       1,1,-1,
       1,1,1,
       -1,1,1,
       -1,1,-1};
  vector<double> cube_nods_pre = cube_nods;
  const auto num_surf = 12;
  const auto num_nods = 8;
  
  cout << "[INFO]>>>>>>>>>>>>>>>>>>>SET PLANE<<<<<<<<<<<<<<<<<<" << endl;
  vector<unsigned int> plane_surf = {0, 1, 2};
  vector<double> plane_nods =
      {0, DOUBLE_MAX, -3,
       -DOUBLE_MAX, -DOUBLE_MAX, -3,
       DOUBLE_MAX,-DOUBLE_MAX, -3};

  cout << "[INFO]>>>>>>>>>>>>>>>>>>>COLLISION<<<<<<<<<<<<<<<<<<" << endl;
  auto COLL_ptr = Collision::getInstance();
  COLL_ptr->Transform_Mesh(3, 1, plane_surf, plane_nods, plane_nods, 0);
  COLL_ptr->Transform_Mesh(num_nods, num_surf,
                      cube_surf, cube_nods, cube_nods_pre, 1);

  for(size_t i = 0; i < 5; ++i){
    cout <<endl<<endl<< "time is " << i << endl << " before: " << endl;
    for(auto& p : cube_nods){cout << p << " ";}
    
    for(size_t j = 0; j < num_nods; ++j){
      cube_nods[j*3 + 2] -= 1;
    }
    cout <<endl<< "after : " << endl;
    for(auto& p : cube_nods){cout << p << " ";}
    cout << endl;

    
    COLL_ptr->Collid();
    COLL_ptr->Transform_Mesh(num_nods, num_surf,
                        cube_surf, cube_nods, cube_nods_pre, 1, false);
    
    cube_nods_pre = cube_nods;
  }

  
}

