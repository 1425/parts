#include "subsystem.h"
#include "home.h"
#include "subsystems.h"
#include "part.h"

using namespace std;

string current_user(){
	{
		//In normal operation, this should always be set
		char *s=getenv("REMOTE_USER");
		if(s) return s;
	}
	{
		//When running on dev server w/o user auth enabled
		char *s=getenv("SERVER_NAME");
		if(s) return s;
	}
	//When running directly from command line
	return "no_user";
}

Id new_item(DB db,Table_name const& table){
	run_cmd(db,"INSERT INTO "+table+" VALUES ()");
	auto q=query(db,"SELECT LAST_INSERT_ID()");
	//PRINT(q);
	assert(q.size()==1);
	assert(q[0].size()==1);
	assert(q[0][0]);
	auto id=stoi(*q[0][0]);
	return id;
}

void inner(ostream& o,Subsystem_new const& a,DB db){
	auto id=new_item(db,"subsystem");

	if(a.parent){
		auto q="INSERT INTO subsystem_info ("
			"subsystem_id,"
			"edit_user,"
			"edit_date,"
			"valid,"
			"parent "
			") VALUES ("
			+escape(id)+","
			+escape(current_user())+","
			"now(),"
			"1,"
			+escape(a.parent)
			+")";
		run_cmd(db,q);
	}
	Subsystem_editor page;
	page.id={id};
	make_page(
		o,
		"New subsystem",
		redirect_to(page)
	);
}

string parts_of_subsystem(DB db,Request const& page,Subsystem_id id){
	auto q=qm<Part_id,Part_state,int>(
		db,
		"SELECT part_id,part_state,qty "
		"FROM part_info "
		"WHERE (id) IN "
			"(SELECT MAX(id) FROM part_info GROUP BY part_id) "
			"AND valid AND subsystem="+as_string(id)
	);
	return h2("Parts directly in subsystem")+as_table(db,page,vector<Label>{"Part","State","Qty"},q);
}

string subsystems_of_subsystem(DB db,Request const& page,Subsystem_id subsystem){
	return h2("Subsystems directly in this subsystem")+as_table(
		db,
		page,
		vector<Label>{"Subsystem","Prefix"},
		qm<Subsystem_id,Subsystem_prefix>(
			db,
			"SELECT subsystem_id,prefix "
			"FROM subsystem_info "
			"WHERE "
				"valid AND "
				"id IN (SELECT MAX(id) FROM subsystem_info GROUP BY subsystem_id) AND "
				"parent="+escape(subsystem)
		)
	);
}

void inner(ostream& o,Subsystem_editor const& a,DB db){
	vector<string> info_col_names;
	#define X(A,B) info_col_names|=""#B;
	SUBSYSTEM_INFO_ROW(X)
	#undef X

	vector<string> data_col_names;
	#define X(A,B) data_col_names|=""#B;
	SUBSYSTEM_DATA(X)
	#undef X

	/*auto q=qm<
		#define X(A,B) A,
		SUBSYSTEM_DATA(X)
		#undef X
		Dummy
	>(*/
	auto q=query(
		db,
		"SELECT "+join(",",data_col_names)+
		" FROM subsystem_info "
		"WHERE subsystem_id="+as_string(a.id)+
		" ORDER BY edit_date DESC LIMIT 1"
	);
	Subsystem_data current;
	if(q.size()==0){
		current.valid=1;
	}else{
		assert(q.size()==1);
		auto row=q[0];
		unsigned i=0;
		#define X(A,B) { \
			if(row[i]) current.B=parse((A*)0,*row[i]); \
			i++;\
		}
		SUBSYSTEM_DATA(X)
		#undef X
	}

	make_page(
		o,
		as_string(current.name)+" Subsystem",
		string()+"<form>"
		"<input type=\"hidden\" name=\"p\" value=\"Subsystem_edit\">"
		"<input type=\"hidden\" name=\"subsystem_id\" value=\""+as_string(a.id)+"\">"+
		#define X(A,B) show_input(db,""#B,current.B)+
		SUBSYSTEM_DATA(X)
		#undef X
		"<br><input type=\"submit\">"+
		"</form>"
		+link([=](){ BOM r; r.subsystem=a.id; return r; }(),"BOM")
		+h2("Overview")
		+[=](){
			stringstream ss;
			Subsystem_new sub_new;
			sub_new.parent=a.id;
			ss<<link(sub_new,"New Subsystem")<<" ";
			Part_new part_new;
			part_new.subsystem=a.id;
			ss<<link(part_new,"New Part");
			return ss.str();
		}()
		+"<table border><tr>"+th("Name")+th("Status")+"</tr>"+indent_sub_table(db,0,a.id,{})+"</table>"
		+parts_of_subsystem(db,a,a.id)
		+subsystems_of_subsystem(db,a,a.id)
		+h2("History")
		+make_table(
			a,
			{
				//"edit_date","edit_user","id","name","subsystem_id","valid","parent","prefix"
				#define X(A,B) ""#B,
				SUBSYSTEM_INFO_ROW(X)
				#undef X
			},
			query(
				db,
				"SELECT "+join(",",info_col_names)+
				//"edit_date,edit_user,id,name,subsystem_id,valid,parent,prefix"
				" FROM subsystem_info WHERE subsystem_id="+as_string(a.id)
			)
		)
	);
}

void inner(ostream& o,Subsystem_edit const& a,DB db){
	vector<pair<string,string>> v;
	v|=pair<string,string>("edit_date","now()");
	v|=pair<string,string>("edit_user",escape(current_user()));
	#define X(A,B) if(""#B==string("part_number")) v|=part_entry(db,a.parent,a.B); else v|=pair<string,string>(""#B,escape(a.B));
	SUBSYSTEM_EDIT_ITEMS(X)
	#undef X
	auto q="INSERT INTO subsystem_info ("
		+join(",",firsts(v))
		+") VALUES ("
		+join(",",seconds(v))
		+")";
	//PRINT(q);
	run_cmd(db,q);
	Subsystem_editor page;
	page.id=Subsystem_id{a.subsystem_id};
	make_page(
		o,
		"Subsystem edit",
		redirect_to(page)
	);
}

