//============================================================================
// Name        : mcafee-test-cpp.cpp
// Author      : James Xiang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

//int main() {
//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
//	return 0;
//}

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <climits>
#include <ctime>
#include <thread>
// #include <random>

#include "ts.h"

#define CONNECT_FAILED 1
#define REQUEST_FAILED 2
#define RESPONSE_FAILED 3
#define DOWNLOAD_SUCCEEDED 4


// log file pointer
FILE *log_FP = NULL;

void my_log_function(TS_Log_Level level, TS_Log_Area areas, const char *message);
int rate_url(TS_Handle ts_handle, char *url, int vobersity);
void load_test_data(char *in_file, vector<string> &test_vec, int &num_entries);
void* thread_func (void* parameters);


typedef struct My_Download_Func_Info_t {
	int my_status;
} My_Download_Func_Info;

typedef struct thread_parameters_t {
	TS_Handle ts_handle;
	int duration;
	int vobersity;
	//vector<string> &test_vector;
} Thread_Parameters;

typedef struct thread_return_t {
	long num_queris;
	long num_categorized;
} Thread_Return;

void join_threads(map<pthread_t, Thread_Return> &threads);
void create_joined_results(map<pthread_t, Thread_Return> &threads, int duration, int num_workers, int lookup_mode);


// global test data shared with multiple threads
vector<string> test_vector;

int main(int argc, char *argv[])
{
	cout << "!!!McAfee SDK Test!" << endl; // prints !!!Hello World!!!
	// show the exact command
	for (int i=0; i<argc; i++){
		cout << argv[i] << " ";
	}
	cout<<endl;

	// handle command line arguments

	extern char *optarg;
	extern int optind;
	int c, err = 0;

	// default flags and values for all options
	int vflag = 0, log_flag = 0, dflag = 0, lflag = 0, mflag = 0, wflag = 0, iflag = 0;
	int vobersity = 0;  // 0: low vobersity 1: high vobersity for console output
	TS_Log_Level sdk_log_level = TS_LOG_LEVEL_INFO; // log level for sdk
	char *dname = "./data.db";   // local db will be downloaded
	char *lname = "./data.db";   // local db will be loaded
	int db_load_mode = TS_DATABASE_ACCESS_MEMORY; // TS_DATABASE_ACCESS_DISK = 1
	                                            // TS_DATABASE_ACCESS_MEMORY = 2
	int lookup_mode = 1; // 1: local_only" 2: hybrid 3: network_only
	int num_workers = 1;          // number of worker threads to lookup concurrently
	char *iname = "./test.data";
	int duration = 100; // run test for duration seconds

	static char usage[] =
			"usage: %s [-v verbosity] [-V sdk-log-level 1 no, 5 info] [-d db_name] [-l db_name] [-L db_load_mode] [-m lookup_mode(1,2,3)] [-w num_of_workers] [-i test_data] [-t seconds] [-h?]\n";

	// collect all options
	while ((c = getopt(argc, argv, "v:V:d:l:L:m:w:i:t:h?")) != -1)
	{
		switch (c) {
		case 'v':
			vflag = 1;
			vobersity = atoi(optarg);
			break;
		case 'V':
			log_flag = 1;
			sdk_log_level = (TS_Log_Level)atoi(optarg);
			break;
		case 'd':
			dflag = 1;
			dname = optarg;
			break;
		case 'l':
			lflag = 1;
			lname = optarg;
			break;
		case 'L':
			db_load_mode = atoi(optarg);
			break;
		case 'm':
			mflag = 1;
			lookup_mode = atoi(optarg);
			break;
		case 'w':
			num_workers = atoi(optarg);
			break;
		case 'i':
			iname = optarg;
			break;
		case 't':
			duration = atoi(optarg);
			break;
		case 'h':
		case '?':
			fprintf(stdout, usage, argv[0]);
			exit(0);
		}
	}

	// see what we have
	printf("verbosity = %d\n", vobersity);
	printf("sdk_log_level = %d\n", sdk_log_level);
	printf("dflag = %d\n", dflag);
	printf("download_db_name = %s\n", dname);
	printf("load_db_name = %s\n", lname);
	printf("db_load_mode = %d\n", db_load_mode);
	printf("lookup mode = %d\n", lookup_mode);
	printf("number of worker threads = %d\n", num_workers);
	printf("test data = \"%s\"\n", iname);
	printf("duration = %d\n", duration);


	int num_test_entris = 0;
	load_test_data(iname, test_vector, num_test_entris);
	cout << num_test_entris << " of urls are loaded." << endl;
	cout << "size of test vector: "<< test_vector.size() << endl;

	// thread ip and return result table
	map<pthread_t, Thread_Return> workers;

	//exit(0);

    // init the SDK environment
	TS_Handle ts_handle;

	// set log file
	log_FP = fopen("logfile", "a");
	if (NULL == log_FP)
	{
		log_FP = stdout;
	}


	int download_status;

	if (TS_OK != TS_Init()) {
		printf("TS_Init Failed. Abort.\n");
		return 0;
	}

	if (TS_OK != TS_HandleCreate(
			                      &ts_handle,
								  "SF6S-HH37-G34G-X75H",
								  NULL,
								  "Infoblox",
								  "1"))
	{
		printf("TS_HandleCreate failed. Abort.\n");
		return 0;
	}

	/*
	 * Set the log level to info and log all areas.
	 */
	if (TS_OK != TS_LogLevelSet(
			ts_handle,
			sdk_log_level,
			TS_LOG_AREA_ALL))
	{
		printf("TS_LogFunctionSet failed. Abort.\n");
		TS_HandleDestroy(&ts_handle);
		return 0;
	}

	/*
	 * Set the log function to use my_log_function().
	 */
	if (TS_OK != TS_LogFunctionSet(ts_handle, my_log_function)) {
		printf("TS_LogFunctionSet failed. Abort.\n");
		TS_HandleDestroy(&ts_handle);
		return 0;
	}

	// configure network lookup
	if ((lookup_mode == 2) || (lookup_mode == 3))
	{
		const char *returned_serial = NULL;
		const char *errors = NULL;
		const char *client_cert = NULL;
		const char *client_key = NULL;
		const char *trustedsource_server_cert = NULL;

		if (TS_OK != TS_ActivateTrustedSource(
				ts_handle,
				TS_ACTIVATION_SERVER_DEFAULT,
				NULL,
				NULL,
				&returned_serial,
				&client_cert,
				&client_key,
				&trustedsource_server_cert,
				&errors))
		{
			if (NULL == errors)
			{
				printf("Error during activation\n");
			}
			else
			{
				printf("Error from server: %s\n", errors);
			}
		}

		int client_cert_len = strlen(client_cert);
		int client_key_len = strlen(client_key);
		int server_cert_len = strlen(trustedsource_server_cert);

		if (TS_NET_OK != TS_NetLookupConfigureInternal(
				ts_handle,
				"MyDeviceID",
				TS_NETLOOKUP_SERVER_DEFAULT,
				TS_NETLOOKUP_PORT_DEFAULT,
				client_cert,
				client_cert_len,
				client_key,
				client_key_len,
				trustedsource_server_cert,
				server_cert_len))
		{
			printf("Failed initialize networking. Abort.\n");
			TS_HandleDestroy(&ts_handle);
			return 0;
		}

		if (TS_NET_OK != TS_NetLookupEnable(ts_handle, TS_ENABLE)){
			printf("Failed enabling netlookup. Abort.\n");
			TS_HandleDestroy(&ts_handle);
			return 0;
		}


	}

	// check if need to download the db
	if (dflag)
	{
		cout << "download the category data base to file -> " << dname << endl;
	    // set up the download info
		TS_Database_Download_Func_Info download_info;
		memset(&download_info, 0, sizeof(download_info));

	    // download_info.serial_number = "SF6S-HH37-G34G-X75H";
	    // strncpy(download_info.serial_number, "SF6S-HH37-G34G-X75H", 79);
		download_info.database_type = TS_DATABASE_DOWNLOAD_DATABASE_TYPE_XL;
		download_info.ts_handle = ts_handle;


        // Download the complete Web Database, placing
		// the new Web Database file to dname

		if (TS_OK != TS_DatabaseDownload( ts_handle,
										  dname,
										  TS_DATABASE_DOWNLOAD_MODE_FULL,
										  &download_status,
										  &download_info))
		{
			printf("TS_DatabaseDownload failed. Abort.\n");
			TS_HandleDestroy(&ts_handle);
			return 0;
		}

	}


	// Load the local Web Database.
	cout << "Load DB file <- " << lname << " with TS_DatabaseLoad()" << endl;
	TS_Database_Access db_access_mode = TS_Database_Access(db_load_mode);
	if (TS_OK != TS_DatabaseLoad( ts_handle,
								  lname,
								  db_access_mode,
								  TS_CAT_SET_LATEST))
	{
		printf("TS_DatabaseDownload failed. Abort.\n");
		TS_HandleDestroy(&ts_handle);
		return 0;
	}
	else {
		cout << "Local DB is loaded successfully" << endl;
	}

	cout << "create # " << num_workers << " of worker threads with test duration: " << duration << " seconds." <<endl;

	Thread_Parameters th_params;
	th_params.duration = duration;
	th_params.ts_handle = ts_handle;
	th_params.vobersity = vobersity;
	pthread_t thread_id;
	for ( int i=0; i<num_workers; i++)
	{
		pthread_create(&thread_id, NULL, &thread_func, &th_params);
		Thread_Return val;
		val.num_queris = 0;
		val.num_categorized = 0;
		workers[thread_id] = val;
		cout << "thread_id: " << thread_id << " is created!" << endl;
	}

	join_threads(workers);
	create_joined_results(workers, duration, num_workers, lookup_mode);


//	printf("rate a url with rate_url\n");
//	char *url = "google.com";
//	url = "https://www.google.com/intl/en/ads/?fg=1";
//	printf("url: %s\n", url);
//	rate_url(ts_handle, url);

	TS_HandleDestroy(&ts_handle);
	cout << "End of Test!" << endl;
	return 0;
}


/*
 * query url against local DB
 */
int rate_url(TS_Handle ts_handle, char *url, int vobersity)
{

	TS_Url parsed_url;
	TS_Attributes attributes;
	TS_Categories categories;

	char cat_names[4000];
	int len = 0;
	int return_val = 1; // 1 hit, 0: error, -1 un-categorized
	char delimiter[] = ", ";
	int delimiter_len = strlen(delimiter);

	if (TS_OK != TS_AttributesCreate(ts_handle, &attributes)) {
		printf("TS_AttributesCreate failed. Abort.\n");
		TS_HandleDestroy(&ts_handle);
		return 0;
	}

	if (TS_OK != TS_CategoriesCreate(ts_handle, &categories)) {
		printf("TS_CategoriesCreate failed. Abort.\n");
		TS_AttributesDestroy(ts_handle, &attributes);
		TS_HandleDestroy(&ts_handle);
		return 0;
	}

	if (TS_OK != TS_CategoriesCategoryRemoveAll(ts_handle, categories)) {
		printf("TS_CategoriesCategoryRemoveAll failed. Abort.\n");
		TS_AttributesDestroy(ts_handle, &attributes);
		TS_CategoriesDestroy(ts_handle, &categories);
		TS_HandleDestroy(&ts_handle);
		return 0;
	}

	if (TS_OK != TS_UrlCreate(ts_handle, &parsed_url)) {
		printf("TS_UrlCreate failed. Abort.\n");
		TS_AttributesDestroy(ts_handle, &attributes);
		TS_CategoriesDestroy(ts_handle, &categories);
		TS_HandleDestroy(&ts_handle);
		return 0;
	}
	if (TS_OK != TS_UrlParse(ts_handle,
			                 url,
							 NULL,
							 parsed_url)) {
		printf("TS_UrlParse failed. Abort.\n");
		return_val = 0;
		goto done;
	}

	if (TS_OK != TS_RateUrl(
			ts_handle,
			parsed_url,
			attributes,
			categories,
			NULL,
			0,
			TS_CAT_SET_LOADED,
			0,
			NULL))
	{
		printf("TS_RateUrl failed. Abort.\n");
		return_val = 0;
		goto done;
	}

	// Get categories number
	int num_cats;
	if (TS_OK != TS_CategoriesCount(ts_handle, categories, &num_cats)) {
		cout << "Get categories number error!" << endl;
	}

	// Get categories codes
	unsigned cat_array[num_cats+1];
	for (int i=0; i<num_cats+1; i++)
		cat_array[i] = 0;

	// ignore errors
	if (TS_OK != TS_CategoriesToArray(ts_handle, categories, cat_array, &num_cats) ) {
		cout << "Failed to get category code array"<< endl;
	}


	len = sizeof(cat_names) - 1;
	if (TS_OK != TS_CategoriesToString(
			ts_handle,
			categories,
			TS_LANGUAGE_ENGLISH,
			TS_ENCODING_UTF8,
			delimiter,
			delimiter_len,
			cat_names,
			&len))
	{
		printf("TS_CategoriesToString failed. Abort.\n");
	    return_val = 0;
	    goto done;
    } else {
    	cat_names[len] = '\0';

    	if (strlen(cat_names) <= 1){
    		return_val = -1;
    		if (vobersity >= 1)
    		    cout << "x URL: " << url << " is uncategorized!" << endl;
    	} else {
    		if (vobersity >= 2) {
    			//
    			cout << "v URL: " << url << " is categorized as :\t" << cat_names << "; Category Codes: ";
    			for (int i=0; i<num_cats; i++){
    				cout << " " << cat_array[i];
    			}
    			cout << endl;
    		}
    	}
    }

done:
    TS_AttributesDestroy(ts_handle, &attributes);
    TS_CategoriesDestroy(ts_handle, &categories);
    TS_UrlDestroy(ts_handle, &parsed_url);
    //TS_HandleDestroy(&ts_handle);

    return return_val;
}


// Customized log function
void my_log_function(TS_Log_Level level, TS_Log_Area areas, const char *message) {

	if (NULL != log_FP) {
		//
		fprintf(log_FP, "%s\n", message);
		fflush(log_FP);
	}

	if ((TS_LOG_LEVEL_ERROR == level) && (TS_LOG_AREA_DATABASE_LOAD & areas)) {
		syslog(LOG_ERR, "%s\n", message);
	}
	return;
}

void load_test_data(char *in_file, vector<string> &test_vec, int &num_entries)
{
	cout << "load test data from file <- " << in_file << endl;
	// open file stream in read mode
	ifstream infile(in_file);
	string url;
	if (! infile){
		cerr << "open file : " << in_file << " error or file does not exist!" << endl;
		exit(1);
	}
	else {
		// read lines from input file to test vector
		while (infile >> url)
		{
			test_vec.push_back(url);
			num_entries++;
		}
	}
	infile.close();
}

// Thread function will be executed by a thread
void* thread_func (void* parameters)
{
	cout << "execute thread function" << endl;
	Thread_Return *th_ret = (Thread_Return*)malloc(sizeof(Thread_Return));
	th_ret->num_queris = 0;
	th_ret->num_categorized = 0;
	Thread_Parameters *pParam = (Thread_Parameters *)parameters;
    int duration = pParam->duration;
	time_t start_time, current_time;
	time(&start_time);
	cout << "start_time :" << start_time << endl;
	cout << "ctime: " << ctime(&start_time) << endl;

	// get a random url from the test_vector
	srand( time(NULL) ); //initialize the random seed
	int random_index;
	int range = test_vector.size();

	time(&current_time);
	while (current_time < start_time + duration)
	{
		random_index = rand() % range;
		string url_str = test_vector[random_index];
		// convert string as char *
		vector<char> chars;

		copy( url_str.begin(), url_str.end(), back_inserter(chars));
		chars.push_back('\0');
		char *url = &chars[0];

		//cout << "query random  test_vector[" << random_index << "] = "
			// << url_str << endl;
		//cout << "char* = " << url << endl;
        int hit = 0;
		hit = rate_url(pParam->ts_handle, url, pParam->vobersity);

		if (hit == 1) {
			th_ret->num_categorized++;
		}
		th_ret->num_queris++;
		time(&current_time);
	}
	pthread_t self_id = pthread_self();
	cout << "# thread_id: " << self_id << " # number of queries: " << th_ret->num_queris
	     << " categorized: " << th_ret->num_categorized << endl;
	return (void*) th_ret;
}

void join_threads(map<pthread_t, Thread_Return> &threads)
{
	cout << "Join threads and save returns to table ..." << endl;
	map<pthread_t, Thread_Return>::iterator it = threads.begin();
	pthread_t th_id;
	for (; it != threads.end(); ++it)
	{
		th_id = it->first;
		// cout << "thead_id:" << th_id
			// << "  Value: " << it->second << endl;
		//long ret=0;

		Thread_Return *ret = (Thread_Return *)malloc(sizeof(Thread_Return));
		ret->num_queris = 0;
		ret->num_categorized = 0;

		pthread_join(th_id, (void**) &ret);
		cout << "ret->num_queris = " << ret->num_queris <<endl;
		threads[th_id].num_queris = ret->num_queris;
		threads[th_id].num_categorized = ret->num_categorized;
		free(ret);
		cout << "# Join test results:" << endl;
		cout << "# thread_id: " << th_id << " # number of queries: " << threads[th_id].num_queris << endl;
		cout << " number of categorized: " << threads[th_id].num_categorized << endl;
	}

}

void create_joined_results(map<pthread_t, Thread_Return> &threads, int duration, int num_workers, int lookup_mode)
{
	cout << "# Joined Test Result:" << endl;
	map<pthread_t, Thread_Return>::iterator it = threads.begin();
	pthread_t th_id;
	Thread_Return ret;
	unsigned long long sum = 0;
	unsigned long long num_categoriezed = 0;

	for (; it != threads.end(); ++it) {
		th_id = it->first;
		ret = it->second;
		sum += ret.num_queris;
		num_categoriezed += ret.num_categorized;
		cout << "# thread_id: " << th_id << " # number of queries: " << ret.num_queris
			 << " Number of Categorized: " << ret.num_categorized << endl;
	}
	cout << "total queries: " << sum << " in " << duration << " seconds." << endl;
	cout << "total number of categorized: " << num_categoriezed << endl;
	long qps = sum / (duration * 1000);  // k
	float categorized_rate = num_categoriezed/(sum*1.0);
	string mode;
	switch (lookup_mode)
	{
		case 1:
			mode = "Local-Only";
			break;
		case 2:
			mode = "Hybrid";
			break;
		case 3:
			mode = "Network-Only";
			break;
	}
	cout << "\n#Lookup Mode: " << mode << "\n"
		 << "Number of Workers: " << num_workers << " "
		 << "QPS(K) = " << qps << " "
		 << "Categorized Rate: " << categorized_rate*100 << "%" << endl;
	ofstream outfile("./test_results.txt", ios_base::app);
	outfile << "\n#Lookup Mode: " << mode
			<< "\nNumber of threads: " << num_workers << "\tQPS(K) = " << qps << " "
			<< "Categorized Rate: " << categorized_rate*100 << "%" << endl;
	outfile.close();

}








