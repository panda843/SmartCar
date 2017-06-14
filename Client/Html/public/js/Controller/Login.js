var Login = {};

Login.Construct = function(app){
    app.controller('LoginCtl',function($scope,$document,User,Route,$http) {
        if(User.isLogin()){
            Route.Redirect("index");
        }
        var sendLoginPostData = function(){
            $scope.success = true;
            $scope.error = true;
            var name = String($scope.username);
            if(name.length <= 0 || $scope.username == undefined){
                $scope.error = false;
                $scope.message = "账号不能为空";
                return;
            }
            var pass = String($scope.password);
            if(pass.length <= 0 || $scope.password == undefined){
                $scope.error = false;
                $scope.message = "密码不能为空";
                return;
            }
            $http({
                method:'POST',
                url:GankTools.api_url+"/user/login",
                params:{username:$scope.username,password:$scope.password}
            }).then(function successCallback(response) {
                if(response.data.status){
                    $scope.message = "登录成功";
                    $scope.success = false;
                    User.setToken(response.data.data.token);
                    User.setId(response.data.data.user_id);
                    User.setUserName($scope.username);
                    User.setNickName(response.data.data.nickname);
                    User.setHead(response.data.data.head);
                    Route.Redirect("index");
                }else{
                    $scope.error = false;
                    $scope.message = response.data.message;
                }
            }).catch(User.checkRequestCallback);
        }
        $document.bind("keypress", function(event) {
            $scope.$apply(function (){
                var keycode = window.event?event.keyCode:event.which;
                if(keycode==13){
                    sendLoginPostData();
                }
            })
        });
        $scope.submit = function(){
            sendLoginPostData();
        }
    });
}