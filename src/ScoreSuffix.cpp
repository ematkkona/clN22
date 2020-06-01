// Modified for "clN22 - OpenCL accelerated replacement for 'zold-score'-worker"
// clN22-score_suffix-v0.81 (c)2019 EM~ eetumk@kkona.xyz

// Copyright (c) 2018-2019 Yegor Bugayenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the 'Software'), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdio>
#include <cstring>
#include <array>
#include <random>
#include <vector>
#include <openssl/sha.h>
#include <ruby.h>
#include <ruby/thread.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <iosfwd>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

static std::uint32_t ttExpire = 1500;                                    // set timeout waiting to get assigned to a worker - seconds
static std::uint32_t ttExpWork = 700;					 // set timeout for assigned workitem - seconds
static std::string BaseUrl = "http://kkona.xyz:8080/workitem";           // url to 'clN22 Manager' format: "http://<HOST>:<PORT>/workitem"

struct index_params {
        string prefix;
        int strength;
        string nonce;
        bool keep_going;
};

static
array<uint8_t, SHA256_DIGEST_LENGTH> sha256(const string &string)
{
        SHA256_CTX ctx;
        SHA256_Init(&ctx);
        SHA256_Update(&ctx, string.data(), string.size());
        array<uint8_t, SHA256_DIGEST_LENGTH> hash;
        SHA256_Final(&hash[0], &ctx);
        return hash;
}

static
std::int32_t expireItem(std::string whyExp, std::string cWorkUrl)
{
        for (int a=0; a<3; a++) {
                std::string resultExp = "{ \"result\": \"expired\" }";
                auto r = cpr::Put(cpr::Url{ cWorkUrl },
                cpr::Body{ resultExp },
                cpr::Header{ { "Content-Type", "application/json" } });
                if (r.status_code == 200) {
                     std::cout << "[clN22] Workitem expired due to " << whyExp  << std::endl << "[clN22] Make sure clN22-manager has enough active workers. Lower the number of nodes allowedd." << std::endl;
                     return 0;
                 }
                 else {
                     std::cout << "[clN22] Expired: Error trying to expire work! Retry " << (a+1) << "/3..." << std::endl;
                     std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                     if (a>=2) {
                         cout << "[clN22] Error! Unable to flag workitem 'expired'. Make sure 'clN22 Manager' is running." << std::endl;
                         return 1;
                     }
                 }
         }
}

static
std::string clNworker(std::string prfix)
{
        using namespace std::chrono;
        std::int32_t ttExp=0;
	std::int32_t ttExpW=0;
        std::int32_t proceed=0;
        std::string workId = "";
        std::string workCmp = "";
        std::string workIn = prfix;
        std::string resultOut = "";
        std::string cWorkUrl = "";
        std::string clnResult = "";
        std::string postNewBd = "{ \"workitem\": \"" + prfix + "\", \"result\": \"NONE\" }";
        auto r = cpr::Post(cpr::Url{ BaseUrl },
        cpr::Body{ postNewBd },
        cpr::Header{ { "Content-Type", "application/json" } });
        if (r.status_code == 200) {
                json retrieved = json::parse(r.text, nullptr, false);
               if (retrieved.is_discarded()) {
                        cout << "[clN22] PostJob: Error parsing JSON string.";
                }
                else {
                workId = retrieved["id"];
                cWorkUrl = BaseUrl + "/" + workId;
                cout << "[clN22] Workitem: " << prfix << std::endl << "[clN22] This workitem is set to expire in " << ttExpire << "s." << std::endl;
                }
        }
        else {
                cout << "[clN22] PostJob: Error posting work! Sleep for 5 seconds ..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                clnResult = "parserr";
		proceed++;
                
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        auto start = std::chrono::steady_clock::now();
	auto tInstWork = std::chrono::steady_clock::now();
        std::int32_t progress = 0;

        while(proceed==0) {
                auto r2 = cpr::Get(cpr::Url{ BaseUrl });
                json retRes = json::parse(r2.text, nullptr, false);
                if (retRes.is_discarded()) {
                        cout << "[clN22] GetResult: Error parsing JSON string." << std::endl;
			return "parserr";
			break;
                }
                else {
  	              auto& messages = retRes["message"];
        	        for (auto& workitem : messages)
                	{
				if (workitem.contains("result")) {
        	                	 workCmp = workitem["_id"];
                	                 clnResult = workitem["result"];
                        	        if (workCmp == workId) {
                                		if(clnResult != "NONE" && clnResult != "working" && clnResult != "expired" && clnResult != "bailedw") {
                                        		auto end = std::chrono::steady_clock::now();
                                                	cout << "[clN22] Yay! Result received! Time: " << std::chrono::duration_cast<seconds>(end-start).count() << "s. Result: " << clnResult << std::endl;
	                                                auto r3 = cpr::Delete(cpr::Url{ cWorkUrl });
        	                                        return clnResult;
						}
                        	                        else if (clnResult == "working") {
								if (progress == 1) {
	        	                	                        auto endw = std::chrono::steady_clock::now();
        	        	                	                ttExpW = std::chrono::duration_cast<seconds>(endw-start).count();
                                	                	        if (ttExpW >= ttExpWork) {
                	                	                	        proceed = expireItem("utterly ueseless niilo-tier worker. Mrr...", cWorkUrl);
	                                                	                return("expred");
	        	                                                }
        	        	                                }
								else {
									start = std::chrono::steady_clock::now();
									cout << "[clN22] Assigned to worker. Timeout: " << ttExpWork << "s." << std::endl;
									progress = 1;
								}
			                       			 if (progress > 0) {
        			                        		auto end = std::chrono::steady_clock::now();
					                                ttExp = std::chrono::duration_cast<seconds>(end-start).count();
        		        				        if (ttExp >= ttExpire) {
										proceed = expireItem("not being assigned to worker in time.", cWorkUrl);
                        	                				return("exprdw");
			                              			}
        	                				}
							}
							else if (clnResult == "bailedw") {
								proceed = expireItem("worker being lost. Mrr..", cWorkUrl);
								return("bailed");
							}
        	        		}
	        		}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(8000));
		}
	}
}	

static
bool check_hash(const array<uint8_t, SHA256_DIGEST_LENGTH> &hash, int strength)
{
        int current_strength = 0;
        const auto rend = hash.rend();
	for (auto h = hash.rbegin(); h != rend; ++h) {
		if ((*h & 0x0f) != 0) {
			break;
		}
		current_strength += (*h == 0) ? 2 : 1;
		if (*h != 0) {
			break;
		}
	}
	return current_strength >= strength;
}

static
string create_nonce(uint64_t i)
{
	const string chars =
		"0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	string rv;
	for (int l = 0; l < 6; l++) {  // Cut to 6 sym
		rv += chars[i % chars.size()];
		if (i < chars.size()) {
			break;
		}
		i /= chars.size();
	}
	return {rv.rbegin(), rv.rend()};
}

static
void *index(void *arg)
{
        std::string resultIN = "";
        index_params *params = static_cast<index_params *>(arg);
        mt19937_64 random(uint64_t(time(nullptr)));
        for (uint64_t i = random(); params->keep_going; i++) {
                resultIN = clNworker(params->prefix);
                const auto hash = sha256(params->prefix + " " + resultIN);
                params->nonce = resultIN;
                if (check_hash(hash, params->strength)) {
                        break;
                }
                else {
                        cout << endl << "[clN22] Invalid result: " << params->prefix << " " << params->nonce << endl;
                }
        }
        return nullptr;
}


static
void unblocking_func(void *arg)
{
	index_params *params = static_cast<index_params *>(arg);
	params->keep_going = false;
}

static
VALUE ScoreSuffix_initialize(VALUE self, VALUE prefix, VALUE strength)
{
	rb_iv_set(self, "@prefix", prefix);
	rb_iv_set(self, "@strength", strength);
	return self;
}

static
VALUE ScoreSuffix_value(VALUE self)
{
	auto prefix_value = rb_iv_get(self, "@prefix");
	index_params params = {
		StringValuePtr(prefix_value),
		NUM2INT(rb_iv_get(self, "@strength")),
		"",
		true
	};
	rb_thread_call_without_gvl(index, &params, unblocking_func, &params);
	return rb_str_new2(params.nonce.c_str());
}

extern "C"
void Init_score_suffix()
{
	VALUE module = rb_define_module("Zold");
	VALUE score_suffix = rb_define_class_under(
		module,
		"ScoreSuffix",
		rb_cObject
	);
	rb_define_method(
		score_suffix,
		"initialize",
		reinterpret_cast<VALUE(*)(...)>(ScoreSuffix_initialize),
		2
	);
	rb_define_method(
		score_suffix,
		"value",
		reinterpret_cast<VALUE(*)(...)>(ScoreSuffix_value),
		0
	);
}

