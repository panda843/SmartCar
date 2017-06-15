var Control = {};

Control.Construct = function(app){
    app.controller('ControlCtl',function($scope,$document,User,Cookie,Route,Video,$http){
        //检查是否登录
        if(!User.isLogin()){ Route.Redirect("login"); }
        $scope.url_console = Route.getRedirectUrl("index");
        $scope.nickname = User.getNickName();
        $scope.logout = User.logOut;
        Video.initVideo();
        //设置基本信息
        $http.get(GankTools.api_url+"/device/info"+"?token="+User.getToken()+"&sockfd="+Cookie.getCookie("control_sockfd")).then(function successCallback(response) {
            $scope.mem_total = response.data.data.mem_total;
            $scope.mem_used = response.data.data.mem_used;
            $scope.disk_total = response.data.data.disk_total;
            $scope.disk_used = response.data.data.disk_used;
            if(response.data.data.video_enable){
                if(Video.isFlash()){
                    Video.setUrl(response.data.data.video_rtmp,'rtmp');
                }else{
                    Video.setUrl(response.data.data.video_hls,'hls');
                }
                Video.start();
            }
        }).catch(User.checkRequestCallback);
        //按键绑定
        $document.bind("keypress", function(event) {
            $scope.$apply(function (){
                var key = window.event?event.keyCode:event.which;
                $http.get(GankTools.api_url+"/device/keypress"+"?token="+User.getToken()+"&sockfd="+Cookie.getCookie("control_sockfd")+"&key="+key).then(function successCallback(response) {
                    console.log(response.data);
                }).catch(User.checkRequestCallback);
            });
        });
        //相机开关
        $scope.setCameraPower = function(){
            $http.get(GankTools.api_url+"/camera/power"+"?token="+User.getToken()+"&sockfd="+Cookie.getCookie("control_sockfd")).then(function successCallback(response) {
                if(response.data.data.video_enable){
                    if(Video.isFlash()){
                        Video.setUrl(response.data.data.video_rtmp,'rtmp');
                    }else{
                        Video.setUrl(response.data.data.video_hls,'hls');
                    }
                    Video.start();
                }else{
                    Video.stop();
                }
            }).catch(User.checkRequestCallback);
        }
    });
}