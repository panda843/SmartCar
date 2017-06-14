var Video = {};

Video.Construct = function(app){
    app.factory('Video', function() {
        //video对象  
        var Video = {};
        //SWF控件
        videojs.options.flash.swf = "https://cdn.bootcss.com/video.js/6.0.1/video-js.swf";
        //video_id
        var object = videojs('smartVideo');
        //mime
        var mime = {'hls':'application/x-mpegURL','rtmp':'rtmp/flv'};
        //添加菜单
        Video.initMenu = function(){
            object.size(760,299);
            object.ready(function() {
                $videoPanelMenu = $(".vjs-fullscreen-control");  
                $videoPanelMenu.before('<div class="vjs-subtitles-button vjs-menu-button vjs-menu-button-popup vjs-control vjs-button" tabindex="0" role="menuitem" aria-live="polite" aria-expanded="false" aria-haspopup="true">'  
                    + '<div class="vjs-menu" role="presentation">'  
                    + '<ul class="vjs-menu-content" role="menu">'  
                    + '<li class="vjs-menu-item" tabindex="-1" role="menuitemcheckbox"  onclick="changeUrl(this)">高清</li>'  
                    + '<li class="vjs-menu-item vjs-selected" tabindex="-1" role="menuitemcheckbox"  onclick="changeUrl(this)">标清 </li>'  
                    + '</ul></div><span class="vjs-control-text">清晰度</span></div>');  
            });
        }
        //检查是否支持flash
        Video.isFlash = function(){
            var hasFlash = 0; //是否安装了flash
            var flashVersion = 0; //flash版本
            if (document.all) {
                var swf = new ActiveXObject('ShockwaveFlash.ShockwaveFlash');
                if (swf) {
                    hasFlash = 1;
                    VSwf = swf.GetVariable("$version");
                    flashVersion = parseInt(VSwf.split(" ")[1].split(",")[0]);
                }
            } else {
                if(navigator.plugins && navigator.plugins.length > 0) {
                    var swf = navigator.plugins["Shockwave Flash"];
                    if (swf) {
                        hasFlash = 1;
                        var words = swf.description.split(" ");
                        for (var i = 0; i < words.length; ++i) {
                            if (isNaN(parseInt(words[i]))) continue;
                            flashVersion = parseInt(words[i]);
                        }
                    }
                }
            }
            return hasFlash;
        }
        //设置播放地址
        Video.setUrl = function(url,type){
            object.src({
                src: url,
                type: mime[type]
            });
        }
        //开始播放
        Video.start = function(){
            object.play();
        }
        //暂停播放
        Video.stop = function(){
            object.pause();
        }
        //重载视频
        Video.reload = function(){
            object.load();
        }
        return Video;
    });
};