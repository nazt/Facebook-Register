<?php
require_once 'lib/facebook.php';

function authen() {
  // Create our Application instance (replace this with your appId and secret).
  $facebook = new Facebook(array(
    'appId'  => '153773681301398',
    'secret' => '29045b13a203eed263447fb8f4959bfb',
    'cookie' => true,
  ));
  // We may or may not have this data based on a $_GET or $_COOKIE based session.
  //
  // If we get a session here, it means we found a correctly signed session using
  // the Application Secret only Facebook and the Application know. We dont know
  // if it is still valid until we make an API call using the session. A session
  // can become invalid if it has already expired (should not be getting the
  // session back in this case) or if the user logged out of Facebook.
  $me = null;
  $session = $facebook->getSession();

  // login or logout url will be needed depending on current user state.
  if ($me) {
    $logoutUrl = $facebook->getLogoutUrl();
  } else {
    $loginUrl = $facebook->getLoginUrl(array(
              'canvas'    => 1,
              'fbconnect' => 0,
              'req_perms' => 'email,publish_stream,status_update,user_birthday,user_location,user_work_history'
              ));
  }
  // Session based API call.
  if ($session) {
   // echo "<script type='text/javascript'>top.location.href = '$loginUrl';</script>";
    try {
      $uid = $facebook->getUser();
      $me = $facebook->api('/me');
    } catch (FacebookApiException $e) {
      error_log($e);
    }
  } else {
    drupal_set_html_head( "<script type='text/javascript'>top.location.href = '$loginUrl';</script>");
    //echo "<script type='text/javascript'>top.location.href = '$loginUrl';</script>";
  }
  // This call will always work since we are fetching public data.
  // $naitik = $facebook->api('/naitik');
    drupal_add_js("
      window.fbAsyncInit = function() {
        FB.init({
          appId   : '". $facebook->getAppId() ."',
          session : ". json_encode($session) .", // don't refetch the session when PHP already has it
          status  : true, // check login statuss

          cookie  : true, // enable cookies to allow the server to access the session
          xfbml   : true // parse XFBML
        });

        // whenever the user logs in, we refresh the page
        FB.Event.subscribe('auth.login', function() {
         window.location.reload();
        });
      };
     $(document).ready(function() {  
      (function() {
         var e = document.createElement('script');
         e.src = document.location.protocol + '//connect.facebook.net/en_US/all.js';
         e.async = true;
         document.getElementById('fb-root').appendChild(e);
       }());
    
      $('#disconnect').bind('click', function() {
            FB.api({ method: 'Auth.revokeAuthorization' }, function(response) {
              if(response===true) {
                document.location = 'http://nat.opendream.in.th/drupal/node/1';
              }
            });    
       }); 
      });", "inline");
//      drupal_set_message(print_r($me, 1));
      $account = array(
        'name' => $me[id],
        'mail' => $me[email],
        'fb-name' => $me[name],
        'timezone' => variable_get('date_default_timezone', NULL), 
        'pass' => md5(json_encode($session)) , 
        'init' => $user->email, 
        'roles' => array(),
        'status' => 1 
      ); 
      global $user;
      $c = user_load(array('name' => $me[id]));
      if($c == null) {
        $user = user_save('', $account);
      }
      else { 
        drupal_set_message(print_r($c, 1));
        $user = $c;
        drupal_set_message('auto login');
      }
  drupal_set_message('Hello, Drupal\'s World');
  return '<div id="fb-root"></div>
      <div>
    <button id="disconnect">Disconnect</button>
  </div>'. '<pre>' .  print_r($me, 1) . '</pre>';
}
