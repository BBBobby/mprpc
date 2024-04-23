#include"mprpccontroller.h"

MprpcController::MprpcController(){
    isFailed = false;
    errTest = "";
}

void MprpcController::Reset(){
    isFailed = false;
    errTest = "";
}

bool MprpcController::Failed() const{
    return isFailed;
}

std::string MprpcController::ErrorText() const{
    return errTest;

}
void MprpcController::SetFailed(const std::string& reason){
    isFailed = true;
    errTest = reason;
}

void MprpcController::StartCancel(){}
bool MprpcController::IsCanceled() const{return false;}
void MprpcController::NotifyOnCancel(google::protobuf::Closure* callback){}