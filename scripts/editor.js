var sources = new Array;
sources[0] = "editor.html";
sources[1] = "gecko.html";
sources[2] = "gecko.html";
sources[3] = "";

function IsCompatibleBrowser()
{
	var sAgent = navigator.userAgent.toLowerCase() ;
	
	// 1:Internet Explorer
	if ( sAgent.indexOf("msie") != -1 && sAgent.indexOf("mac") == -1 && sAgent.indexOf("opera") == -1 ) {
		var sBrowserVersion = navigator.appVersion.match(/MSIE (.\..)/)[1] ;
		if ( sBrowserVersion >= 5.5 )
			return 1;
		return 0 ;
	}
	
	// 2:Gecko
	if ( navigator.product == "Gecko" && navigator.productSub >= 20030210 )
		return 2 ;
	
	// 3:Opera
	var aMatch = sAgent.match( /^opera\/(\d+\.\d+)/ ) ;
	if ( aMatch && aMatch[1] >= 9.0 )
		return 3 ;
	
	// 4:Safari
	if ( sAgent.indexOf( 'safari' ) != -1 ) {
		if ( sAgent.match( /safari\/(\d+)/ )[1] >= 312 )	// Build must be at least 312 (1.3)
			return 4;
	}

	return 0 ;
}

function InsertHtmlBefore( html, element )
{
	if ( element.insertAdjacentHTML ) {	// IE
		element.insertAdjacentHTML( 'beforeBegin', html ) ;
	} else {							// Gecko
		var oRange = document.createRange() ;
		oRange.setStartBefore( element ) ;
		var oFragment = oRange.createContextualFragment( html );
		element.parentNode.insertBefore( oFragment, element ) ;
	}
}

function SetupEditor(base, areas)
{
	var i =0;
	var br = IsCompatibleBrowser();
	while (areas[i]) {
		if (br && sources[br -1] && sources[br -1] != "") {
			document.getElementById(areas[i]).style.display = 'none' ;
			InsertHtmlBefore( "<iframe id=\"" + areas[i] + "___Frame\" src=\"" + base + "/" + sources[br -1] + "?InstanceName=" + areas[i] + "&ScriptBase=" + base + "\" width=\"100%\" height=\"200\" frameborder=\"0\" scrolling=\"no\" marginwidth=\"0\" marginheight=\"0\"></iframe>", document.getElementById(areas[i]) ) ;
		}
		++i;
	}
}
