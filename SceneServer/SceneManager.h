#pragma once

#include "singleton.h"
#include "Scene.h"
#include "zSceneManager.h"
#include "zUniqueID.h"

class SceneManager: public Singleton<SceneManager>, public zSceneManager
{
public:
	SceneManager();
	~SceneManager();

	bool			init();
	Scene*		loadScene(Scene::SceneType type, const uint32_t country, const uint32_t mapid);

	struct MapInfo                                             
	{                                                          
		///编号                                                  
		DWORD id;                                                
		///名字                                                  
		char name[MAX_NAMESIZE];                                 
		///对应的地图文件名                                      
		char filename[MAX_NAMESIZE];                             
		///玩家在该地图死后回到的地图                            
		DWORD backto;                                            
		///玩家在该地图死后回到的城市                            
		DWORD backtoCity;                                        
		///玩家在外国地图死后回到的城市                          
		DWORD foreignbackto;                                     
		///玩家在公共国地图死后回到的城市                        
		DWORD commoncountrybackto;                               
		///无国家人在外国地图死后回到的城市                      
		DWORD commonuserbackto;                                  
		///国战目的地                                            
		DWORD backtodare;                                        
		///国战期间,在国战战场（目前在王城）死亡后,攻方复活城市  
		DWORD countrydarebackto;                                 
		///国战期间,在国战战场（目前在王城）死亡后,守方复活城市  
		DWORD countrydefbackto;                                  
		///可相互pk的等级                                        
		DWORD pklevel;                                           
		///表示该地图某些功能是否可用,骑马等                     
		DWORD function;                                          
		///表示该地图允许的level级别的玩家进入                   
		BYTE level;                                              
		///表示该地图的经验加成(使用时/100求出比率)              
		BYTE exprate;                                            
	};                                                         
	typedef std::map<DWORD,MapInfo> MapMap;         
	typedef MapMap::value_type MapMap_value_type;   
	typedef MapMap::iterator MapMap_iter;           
	///地图信息和ID的映射                           
	MapMap map_info;                                

	/**
	 * \brief 国家信息
	 *
	 */
	struct CountryInfo
	{
		///编号
		DWORD id;
		///名字
		char name[MAX_NAMESIZE];
		///所在的地图名字
		DWORD mapid;
		///国家功能标识字段
		DWORD function;
	};


	typedef std::map<DWORD,CountryInfo> CountryMap;       
	typedef CountryMap::iterator CountryMap_iter;         
	typedef CountryMap::value_type CountryMap_value_type; 
	///国家信息和ID的映射                                 
	CountryMap country_info;                              
	DWORD getCountryIDByCountryName(const char *name);    

	void loadedSceneNotifySession(Scene*);
	static void registerScript(lua_State* L);
private:
	zUniqueDWORDID *sceneUniqeID;
	bool getUniqeID(DWORD& tempid);
	void putUniqeID(const DWORD& tempid);


};
