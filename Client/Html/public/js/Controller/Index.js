var Index = {};

Index.Construct = function(app){
    app.controller('IndexCtl',function($scope,User,Route,Cookie,$http) {
        if(!User.isLogin()){ Route.Redirect("login"); }
        $scope.url_console = Route.getRedirectUrl("index");
        $scope.nickname = User.getNickName();
        $scope.logout = User.logOut;
        $http.get(GankTools.api_url+"/device/list"+"?token="+User.getToken()).then(function successCallback(response) {
            for(var index in response.data.data){
                var online = response.data.data[index].online;
                var status = response.data.data[index].status;
                response.data.data[index].online = (online == 1) ? "在线":"离线";
                response.data.data[index].status = (status == 1) ? "正常":"异常";
                response.data.data[index].control = (online == 1) ? (status == 1) ? true:false:false;
            } 
            $scope.deviceList = response.data.data;
        }).catch(User.checkRequestCallback);
        
        $scope.redirectControl = function(device_id,sockfd){
            Cookie.setCookie("control_device_id",device_id);
            Cookie.setCookie("control_sockfd",sockfd);
            console.log(sockfd);
            Route.Redirect("control");
        }
    });
}