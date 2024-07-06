/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "powermeter.h"

_powermeter::_powermeter() {

    this->last_save = time(0);
    this->tarif1 = 0;
    this->tarif2 = 0;
    this->tarif_total = 0;
    this->tarif1Returned = 0;
    this->tarif2Returned = 0;
    this->totalReturned = 0;
    this->current = 0;
    this->returned = 0;
    this->volt_l1 = 0;
    this->volt_l2 = 0;
    this->volt_l3 = 0;
    this->amp_l1 = 0;
    this->amp_l2 = 0;
    this->amp_l3 = 0;
    this->gas = 0;
    this->cur_solar = 0;

    this->lifetime_solar = 0;

    this->solar = new PowerMeterSolar(this);
    this->solar->start();
};

_powermeter::~_powermeter() {
    int bp = 0;
    bp++;
};

void _powermeter::save_data() {
    lock_guard<mutex>lg(m);
    time_t nu = time(0);
    time_t diff = nu - this->last_save;
    if (diff > 60) {
        this->last_save = nu;
        if ((this->tarif_total != 0) && (this->gas != 0) &&(this->lifetime_solar != 0)) {


            time_t t = time(NULL);
            // t = convert_to_local_time(t);
            struct tm nu = *localtime(&t);
            int nu_hour = nu.tm_hour;

            int nu_day = nu.tm_yday;
            int nu_year = nu.tm_year;
            int nu_weekday = nu.tm_wday;
            int nu_month = nu.tm_mon;

            int local_hour = nu_hour;
            if (local_hour > 23) {
                nu_day++;
                nu_weekday++;
                if (nu_weekday > 6) {
                    nu_weekday = 0;
                }
                if (nu_day > 365) {
                    nu_day = 0;
                    nu_year++;
                }
                local_hour = local_hour - 24;
            }
            nu_hour = local_hour;

            //check if row exist
            MYSQL *ms = Mysql_Connect();
            if (ms != NULL) {
                string query = "SELECT id FROM powermeter WHERE day = " + inttostring(nu_day) + " AND year = " + inttostring(nu_year) + " AND hour = " + inttostring(nu_hour);
                if (!mysql_query(ms, query.c_str())) {
                    MYSQL_RES *result = mysql_store_result(ms);
                    if (result != NULL) {
                        //Get the number of columns
                        int num_rows = mysql_num_rows(result);
                        int num_fields = mysql_num_fields(result);


                        if (num_rows == 0) {
                            string query2 = "INSERT INTO powermeter(day,year,month, hour, weekday, total, tarif_1, tarif_2, gas, totalReturned, tarif1Returned, tarif2Returned, Solar) VALUES(" + inttostring(nu_day) + ", " + inttostring(nu_year) + "," + inttostring(nu_month) + "," + inttostring(nu_hour) + "," + inttostring(nu_weekday) + "," + longtostring(this->tarif_total) + "," + longtostring(this->tarif1) + "," + longtostring(this->tarif2) + "," + longtostring(this->gas) + ", " + longtostring(this->totalReturned) + ", " + longtostring(this->tarif1Returned) + ", " + longtostring(this->tarif2Returned) + ", " + longtostring(this->lifetime_solar) + ")";
                            if (!mysql_query(ms, query2.c_str())) {

                            } else {
                                string ms_err = string(mysql_error(ms));
                                cout << "MYSQL Error " << ms_err << endl;
                            }
                        } else {
                            MYSQL_ROW row; //An array of strings
                            uint64_t curID = 0;
                            while ((row = mysql_fetch_row(result))) {
                                if (num_fields >= 1) {
                                    uint64_t msid = atol(row[0]);
                                    if(msid > curID){
                                        curID = msid;
                                    } 
                                }
                            }
                            
                            int64_t deleteFrom = curID - (5*365*24);
                            int bp = 0;
                            bp++;
                            string query3 = "DELETE FROM powermeter WHERE id < "+ longtostring(deleteFrom);
                             if (!mysql_query(ms, query3.c_str())) {

                            } else {
                                string ms_err = string(mysql_error(ms));
                                cout << "MYSQL Error " << ms_err << endl;
                            }


                            string query2 = "UPDATE powermeter SET total=" + longtostring(this->tarif_total) + ", tarif_1=" + longtostring(this->tarif1) + ",tarif_2=" + longtostring(this->tarif2) + ", gas=" + longtostring(this->gas) + ", totalReturned =" + longtostring(this->totalReturned) + ", tarif1Returned = " + longtostring(this->tarif1Returned) + ", tarif2Returned = " + longtostring(this->tarif2Returned) + ", Solar = " + longtostring(this->lifetime_solar) + " WHERE day=" + inttostring(nu_day) + " AND year=" + inttostring(nu_year) + " AND hour=" + inttostring(nu_hour);
                            if (!mysql_query(ms, query2.c_str())) {

                            } else {
                                string ms_err = string(mysql_error(ms));
                                cout << "MYSQL Error " << ms_err << endl;
                            }

                        }


                        mysql_free_result(result);
                    }
                } else {
                    string ms_err = string(mysql_error(ms));
                    cout << "MYSQL Error " << ms_err << endl;
                }


                Mysql_Disconnect(ms);
            }
        }
    }




}

void _powermeter::quit() {
    lock_guard<mutex>lg(m);
    this->exit = true;


};

void _powermeter::do_work() {

    try {
        this->save_data();
    } catch (exception &e) {
        WriteError("PowerMeter - Loop", e.what());
    }
};

void _powermeter::set_data(uint32_t t1, uint32_t t2, int32_t live, uint32_t gas, uint32_t ret1, uint32_t ret2, uint32_t l1_volt, uint32_t l2_volt, uint32_t l3_volt, uint32_t l1_amp, uint32_t l2_amp, uint32_t l3_amp, uint32_t live_returned) {
    lock_guard<mutex>lg(m);
    this->tarif2 = t1;
    this->tarif1 = t2;
    this->tarif1Returned = ret1;
    this->tarif2Returned = ret2;
    this->current = live;
    this->returned = live_returned;
    this->gas = gas;
    this->tarif_total = this->tarif1 + this->tarif2;
    this->totalReturned = this->tarif1Returned + this->tarif2Returned;
    this->amp_l1 = l1_amp;
    this->amp_l2 = l2_amp;
    this->amp_l3 = l3_amp;
    this->volt_l1 = l1_volt;
    this->volt_l2 = l2_volt;
    this->volt_l3 = l3_volt;

};

void _powermeter::set_solar(uint32_t cur_solar, uint64_t lifetimesolar) {
    lock_guard<mutex>lg(m);
    this->cur_solar = cur_solar;
    this->lifetime_solar = lifetimesolar;
}

string _powermeter::get_json() {
    lock_guard<mutex>lg(m);
    string ret = "";
    // cout << "LOCK AREA GET_JSON_SMALL()" << endl;


    ret = "{\"tarif_1\":" + longtostring(this->tarif1) + ",";
    ret += "\"tarif_2\":" + longtostring(this->tarif2) + ",";
    ret += "\"tarif_total\":" + longtostring(this->tarif_total) + ",";
    ret += "\"returned_1\":" + longtostring(this->tarif1Returned) + ",";
    ret += "\"returend_2\":" + longtostring(this->tarif2Returned) + ",";
    ret += "\"returned_total\":" + longtostring(this->totalReturned) + ",";
    ret += "\"current\":" + longtostring(this->current) + ",";
    ret += "\"currentReturned\":" + longtostring(this->returned) + ",";
    ret += "\"volt_l1\":" + longtostring(this->volt_l1) + ",";
    ret += "\"volt_l2\":" + longtostring(this->volt_l2) + ",";
    ret += "\"volt_l3\":" + longtostring(this->volt_l3) + ",";
    ret += "\"amp_l1\":" + longtostring(this->amp_l1) + ",";
    ret += "\"amp_l2\":" + longtostring(this->amp_l2) + ",";
    ret += "\"amp_l3\":" + longtostring(this->amp_l3) + ",";
    ret += "\"cur_solar\":" + longtostring(this->cur_solar) + ",";
    ret += "\"gas\":" + longtostring(this->gas) + "}";


    //cout << "UNLOCK AREA GET_JSON_SMALL()" << endl;

    return ret;

}

