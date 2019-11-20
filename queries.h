#ifndef QUERIES_H
#define QUERIES_H

#include "data_types.h"

#define SUBSYSTEM_DATA(X)\
	X(bool,valid)\
	X(std::string,name)\

#define SUBSYSTEM_ROW(X)\
	X(Id,id)\

#define SUBSYSTEM_INFO_ROW(X)\
	X(Id,id)\
	X(Id,subsystem_id)\
	X(User,edit_user)\
	X(Datetime,edit_date)\
	SUBSYSTEM_DATA(X)

#define PART_ROW(X)\
	X(Id,id)\

#define PART_DATA_INNER(X)\
	X(Subsystem_id,subsystem)\
	X(std::string,name)\
	X(Part_number,part_number)\
	X(Part_state,part_state)\
	X(std::string,length)\
	X(std::string,width)\
	X(std::string,thickness)\
	X(Material,material)\
	X(unsigned,qty)\
	X(Decimal,time)\
	X(Date,manufacture_date)\
	X(User,who_manufacture)\
	X(Machine,machine)\
	X(std::string,place)\
	X(std::string,bent)\
	X(Bend_type,bend_type)\
	X(URL,drawing_link)\
	X(URL,cam_link)\
	X(Supplier,part_supplier)\
	X(URL,part_link)\
	X(Date,arrival_date)\
	X(Decimal,price)

#define PART_DATA(X)\
	X(bool,valid)\
	PART_DATA_INNER(X)

struct Part_data{
	PART_DATA(INST)
};

#define PART_INFO_ROW(X)\
	X(Id,id)\
	X(Part_id,part_id)\
	X(User,edit_user)\
	X(Datetime,edit_date)\
	PART_DATA(X)

#define MEETING_ROW(X)\
	X(Id,id)

using Meeting_length=int;
using Color=std::string;

#define MEETING_DATA(X)\
	X(bool,valid)\
	X(Date,date)\
	X(Meeting_length,length)\
	X(Color,color)\
	X(std::string,notes)\

struct Meeting_data{
	MEETING_DATA(INST)
};

#define MEETING_INFO_ROW(X)\
	X(Id,id)\
	X(Meeting_id,meeting_id)\
	X(User,edit_user)\
	X(Datetime,edit_date)\
	MEETING_DATA(X)

using Column_type=std::pair<std::string,bool>; //type and primary key
using Table_type=std::vector<std::pair<std::string,Column_type>>;

Table_type read(DB db,std::string name);

using Table_name=std::string;

void check_table(DB,Table_name,Table_type);
std::set<Table_name> show_tables(DB);

std::string to_db_type(const bool*);
std::string to_db_type(const unsigned*);
std::string to_db_type(const std::string*);
std::string to_db_type(const double*);

void create_table(DB,Table_name,Table_type);

void check_database(DB);

using P=std::map<std::string,std::vector<std::string>>;

std::string to_query(std::map<std::string,std::string> m);

unsigned parse(const unsigned*,std::string);

bool parse(const bool*,std::string);
float parse(const float*,std::string);
double parse(const double*,std::string);

unsigned rand(const unsigned*);

#define STR(X) ""#X

#define PARSE_ITEM(A,B) { \
	auto f=p.find(""#B); \
	if(f==p.end()){\
		if(""#A==string("bool")){\
			r.B=A{};\
		}else{\
			throw string()+"Failed to find:"#B+as_string(p);\
		}\
	}else{\
		if(f->second.size()!=1){ throw "Multiple values for "#B; }\
		try{\
			r.B=parse((A*)nullptr,f->second[0]);\
		}catch(...){\
			throw string()+"Failed to parse \""#B+"\" as an "#A+": \""+f->second[0]+"\""; \
		}\
		p=without_key(p,string(""#B));\
	}\
}

#define TO_Q(A,B) r[""#B]=as_string(a.B);
#define RAND(A,B) rand((const A*)nullptr),
#define INST_EQ(A,B) if(a.B!=b.B) return 0;
#define SHOW(A,B) { o<<""#B<<":"<<a.B<<" "; }
#define DIFF(A,B) if(a.B!=b.B) cout<<""#B<<":"<<a.B<<" "<<b.B<<"\n";

#define DECL_OPTION(T,ITEMS)\
	struct T{ \
		ITEMS(INST) \
	};\
	bool operator==(T const& a,T const& b);\
	bool operator!=(T const& a,T const& b);\
	std::ostream& operator<<(std::ostream& o,T const& a);\
	T rand(const T*);\
	std::string to_query(T a);\
	std::optional<T> parse_query(const T*,P p);\
	void diff(T const& a,T const& b);\

//empty
#define HOME_ITEMS(X) 

#define T Home
DECL_OPTION(Home,HOME_ITEMS)
#undef T

#define SUBSYSTEMS_ITEMS(X)
#define T Subsystems
DECL_OPTION(Subsystems,SUBSYSTEMS_ITEMS)
#undef T

#define SUBSYSTEM_NEW_ITEMS(X)
DECL_OPTION(Subsystem_new,SUBSYSTEM_NEW_ITEMS)

#define SUBSYSTEM_EDITOR_ITEMS(X)\
	X(Subsystem_id,id)
DECL_OPTION(Subsystem_editor,SUBSYSTEM_EDITOR_ITEMS)

#define SUBSYSTEM_EDIT_ITEMS(X)\
	X(Id,subsystem_id)\
	X(bool,valid)\
	X(std::string,name)

DECL_OPTION(Subsystem_edit,SUBSYSTEM_EDIT_ITEMS)

#define PARTS_ITEMS(X)
DECL_OPTION(Parts,PARTS_ITEMS)

#define PART_NEW_ITEMS(X)
DECL_OPTION(Part_new,PART_NEW_ITEMS)

#define PART_EDITOR_ITEMS(X)\
	X(Part_id,id)
DECL_OPTION(Part_editor,PART_EDITOR_ITEMS)
	
#define PART_EDIT_ITEMS(X)\
	X(Id,part_id)\
	PART_DATA(X)
DECL_OPTION(Part_edit,PART_EDIT_ITEMS)

#define CALENDAR_ITEMS(X)
DECL_OPTION(Calendar,CALENDAR_ITEMS)

#define MEETING_NEW_ITEMS(X)
DECL_OPTION(Meeting_new,MEETING_NEW_ITEMS)

#define MEETING_EDITOR_ITEMS(X)\
	X(Meeting_id,id)
DECL_OPTION(Meeting_editor,MEETING_EDITOR_ITEMS)

#define MEETING_EDIT_ITEMS(X)\
	X(Id,meeting_id)\
	MEETING_DATA(X)
DECL_OPTION(Meeting_edit,MEETING_EDIT_ITEMS)

#define ERROR_ITEMS(X)\
	X(std::string,s)
DECL_OPTION(Error,ERROR_ITEMS)

#define BY_USER_ITEMS(X) X(User,user)
DECL_OPTION(By_user,BY_USER_ITEMS)

#define MACHINES_ITEMS(X)
DECL_OPTION(Machines,MACHINES_ITEMS)

#define ORDERS_ITEMS(X)
DECL_OPTION(Orders,ORDERS_ITEMS)

#define MACHINE_ITEMS(X)\
	X(Machine,machine)
DECL_OPTION(Machine_page,MACHINE_ITEMS)

#define STATE_ITEMS(X)\
	X(Part_state,state)
DECL_OPTION(State,STATE_ITEMS)

#define PAGES(X)\
	X(Home)\
	X(Subsystems)\
	X(Subsystem_new)\
	X(Subsystem_editor)\
	X(Subsystem_edit)\
	X(Parts)\
	X(Part_new)\
	X(Part_editor)\
	X(Part_edit)\
	X(Calendar)\
	X(Meeting_new)\
	X(Meeting_editor)\
	X(Meeting_edit)\
	X(By_user)\
	X(Machines)\
	X(Orders)\
	X(Machine_page)\
	X(State)

using Request=std::variant<
	#define X(A) A,
	PAGES(X)
	#undef X
	Error
>;

Request parse_query(std::string);

template<
	#define X(A) typename A,
	PAGES(X)
	#undef X
	typename Z
>
std::string to_query(std::variant<
	#define X(A) A,
	PAGES(X)
	#undef X
	Z
> a){
	#define X(A) if(std::holds_alternative<A>(a)) return to_query(std::get<A>(a));
	PAGES(X)
	X(Z)
	#undef X
	nyi
}

std::string link(Request req,std::string);

#endif
