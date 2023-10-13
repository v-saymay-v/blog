var xborderTopColor = "";
var xborderTopStyle = "";
var xborderTopWidth = "";
var xborderLeftColor = "";
var xborderLeftStyle = "";
var xborderLeftWidth = "";
var xborderRightColor = "";
var xborderRightStyle = "";
var xborderRightWidth = "";
var xborderBottomColor = "";
var xborderBottomStyle = "";
var xborderBottomWidth = "";
var SyNum = 0;
var oTabs = new Object() ;
var oHelps = new Object() ;
oHelps["display"] = "画面にどのように表示されるかを block（ブロック要素）、inline（インライン要素）、list-item（リスト要素）、none（表示しない） のいずれかで指定します。";
oHelps["position"] = "ポジショニングの方法を、static（規定値：本来の位置）、relative（本来の位置からの相対位置指定）、absolute（絶対位置指定）、fixed（絶対位置に固定）、inherit（継承）のいずれかで指定します。fixed は画面のスクロールに対して固定されます。";
oHelps["top"] = "上端からの距離を 10px のような単位付きの数値や 50% のようなパーセント指定、または auto（規定値：自動）、inherit（継承）のいずれかで指定します";
oHelps["left"] = "左端からの距離を 10px のような単位付きの数値や 50% のようなパーセント指定、または auto（規定値：自動）、inherit（継承）のいずれかで指定します";
oHelps["right"] = "右端からの距離を 10px のような単位付きの数値や 50% のようなパーセント指定、または auto（規定値：自動）、inherit（継承）のいずれかで指定します";
oHelps["bottom"] = "下端からの距離を 10px のような単位付きの数値や 50% のようなパーセント指定、または auto（規定値：自動）、inherit（継承）のいずれかで指定します";
oHelps["float"] = "画像などの表示位置を left（左端）、right（右端）、none（規定値：指定しない）、inherit（継承）のいずれかで指定します。";
oHelps["clear"] = "float: によるテキストの回り込みを none（規定値：何もしない）、left（左側の回りこみ解除）、right（右側の回りこみを解除）、both（両側の回りこみを解除）のいずれかで指定します。";
oHelps["z-index"] = "要素同士が重なって表示される場合の、重なりの順序を指定します。z-index に指定した数値が大きなものほど前面に表示されます。";
oHelps["direction"] = "文章の方向（日本語や英語は左から右だが、アラビア語は右から左）を指定します。direction には ltr（規定値：左から右）、rtl（右から左）、inherit（継承）のいずれかを指定します。";
oHelps["unicode-bidi"] = "文章の方向（日本語や英語は左から右だが、アラビア語は右から左）を指定します。bidirection には normal（規定値：通常）、bidi-override（direction を有効にする）、embed（無効にする）、inherit（継承）のいずれかを指定します。";
oHelps["width"] = "横幅を 100px のような単位付き数値、50% のようなパーセント形式、inherit（継承）、auto（規定値：自動）のいずれかで指定します。";
oHelps["height"] = "高さを 100px のような単位付き数値、50% のようなパーセント形式、inherit（継承）、auto（規定値：自動）のいずれかで指定します。";
oHelps["vertical-align"] = "縦方向の配置を、親要素との相対位置で、baseline（ベースラインあわせ）、middle（中央あわせ）、sub（下付き文字の位置）、super（上付き文字の位置）、text-top（テキストの上限あわせ）、text-bottom（テキストの下限あわせ）、top（上端あわせ）、bottom（下端あわせ）、inherit（継承）、70% のようなパーセント形式、0.5em のような単位付きの数値のいずれかで指定します。";
oHelps["overflow"] = "領域をはみ出した要素の扱いを、visible（規定値：表示する）、hidden（隠す）、scroll（スクロールバーで表示する）、auto（自動）、inherit（継承）のいずれかで指定します。";
oHelps["overflow-x"] = "X方向の領域をはみ出した要素の扱いを、visible（規定値：表示する）、hidden（隠す）、scroll（スクロールバーで表示する）、auto（自動）、inherit（継承）のいずれかで指定します。";
oHelps["overflow-y"] = "Y方向の領域をはみ出した要素の扱いを、visible（規定値：表示する）、hidden（隠す）、scroll（スクロールバーで表示する）、auto（自動）、inherit（継承）のいずれかで指定します。";
oHelps["clip"] = "表示する範囲を auto（規定値：自動）、inherit（継承）、rect(上端,右端,下端,左端) 形式のいずれかで指定します。";
oHelps["visibility"] = "表示するしないを、visible（規定値：表示する）、hidden（表示しない）、inherit（継承）のいずれかで指定します。";
oHelps["cursor"] = "この要素にマウスを乗せたときのマウスカーソルの形状を auto（規定値：自動）、crosshair（十字印）、default（通常のもの）、pointer（ポインタ）、move（移動用）、e-resize（右リサイズ）、ne-resize（右上リサイズ）、nw-resize（左上リサイズ）、n-resize（上リサイズ）、se-resize（右下リサイズ）、sw-resize（左下リサイズ）、s-resize（下リサイズ）、w-resize（左リサイズ）、text（テキスト選択）、wait（待ち）、help（ヘルプ）、inherit （継承）のいずれかで指定します。";
oHelps["ime-mode"] = "テキストフォームなどにフォーカスが移動した時の日本語変換（IME）の動作を、auto（規定値：自動）、active（フォーカス移動時にオン）、inactive（フォーカス移動時にオフ）、disabled（IME使用を禁止）のいずれかで指定します。";
oHelps["behavior"] = "スタイルシートでスタイルを一括指定するのに加えて、ビヘイビア機能で動作（振る舞い）を一括指定することができます。behavior には、スクリプトファイルの URL や、<object> タグで指定したID、IE5.0 のデフォルトビヘイビア名などを指定します。";
oHelps["filter"] = "フィルタとは、スタイルシートの拡張として Internet Explorer 4.0 でサポートされた機能です。文字や画像に対して影付きやぼかしなどのエフェクトをかけることができます。";
oHelps["scrollbar-base-color"] = "スクロールバーのすべての要素の色をまとめて設定します。";
oHelps["scrollbar-track-color"] = "スクロールバーの色を指定します。";
oHelps["scrollbar-face-color"] = "スクロールバーの色を指定します。";
oHelps["scrollbar-shadow-color"] = "スクロールバーの色を指定します。";
oHelps["scrollbar-highlight-color"] = "スクロールバーの色を指定します。";
oHelps["scrollbar-3dlight-color"] = "スクロールバーの色を指定します。";
oHelps["scrollbar-arrow-color"] = "スクロールバーの色を指定します。";
oHelps["list-style"] = "<li> タグで表示されるマーカーの形、マーカーの位置、マーカーのイメージを一度に指定します。";
oHelps["list-style-type"] = "<li> タグで表示されるマーカーの形を disc（規定値：黒丸）、circle（白丸）、square（四角）、decimal（1, 2, 3...）、lower-roman（i, ii, iii...）、upper-roman（I, II, III...）、lower-alpha（a, b, c...）、upper-alpha（A, B, C...）、decimal-leading-zero（01, 02, 03...）、lower-greek（α、β、γ...）、upper-greek（Α、Β、Γ...）、lower-latin（小文字ラテン文字）、upper-latin（大文字ラテン文字）、hebrew（ヘブライ数字）、armenian（アルメニア数字）、georgian（グルジア数字）、cjk-ideographic（一、二、三...）、hiragana（あ、い、う...）、katakana（ア、イ、ウ...）、hiragana-iroha（い、ろ、は...）、katakana-iroha（イ、ロ、ハ...）、none（なし）、inherit（継承）のいずれかで指定します。";
oHelps["list-style-position"] = "マーカーの位置を outside（規定値：外側）、inside（内側）、inherit（継承）のいずれかで指定します。";
oHelps["list-style-image"] = "<li> タグで表示されるマーカーのイメージを URL、none（無し）、inherit（継承）のいずれかで指定します。";
oHelps["page-break-before"] = "印刷時に要素の直前で改ページします。";
oHelps["page-break-after"] = "印刷時に要素の直後で改ページします。";
oHelps["background"] = "color, image, repeat, attachment, position を一度に指定します。";
oHelps["background-color"] = "背景色を指定します。color には色の名前、transparent（規定値：透明色）、inherit（継承）のいずれかを指定します。";
oHelps["background-image"] = "背景画像の URL、または none（規定値：画像なし）、inherit（継承）のいずれかを指定します。";
oHelps["background-attachment"] = "ウィンドウのスクロールを動かした時の背景の動作を scroll（規定値：一緒にスクロールする）、fixed（スクロールしない）、inherit（継承）のいずれかで指定します。";
oHelps["background-repeat"] = "背景画像の並べかたを repeat（規定値：敷き詰める）、repeat-x（横方向のみ並べる）、repeat-y（縦方向のみ並べる）、no-repeat（ひとつだけ表示する）、inherit（継承）のいずれかで指定します。";
oHelps["background-position"] = "背景の横方向の位置を left（左端）、center（中央）、right（右端）または 50% のような割合で指定し、縦方向の位置を top（上端）、center（中央）、bottom（下端）または 50% のような割合で指定します。";
oHelps["color"] = "色を指定します。";
oHelps["font-style"] = "normal（規定値：通常）, italic（イタリック）, oblique（斜め）のいずれかを指定します。";
oHelps["font-variant"] = "normal（規定値：通常）, small-caps（大文字）のいずれかを指定します。";
oHelps["font-weight"] = "フォントの太さを normal（規定値）, bold, bolder, lighter, 100, 200, 300, 400, 500, 600, 700, 800, 900 のいずれかで指定します。";
oHelps["font-size"] = "フォントの大きさを指定します。絶対指定として xx-small, x-small, small, medium（規定値）, large, x-large, xx-large、相対指定として larger, smaller、絶対単位指定として 10in, 10cm, 10mm, 10pt, 10pc, 相対単位指定として 10px, 10ex, 10em などを、割合指定として 120% などを指定します。";
oHelps["line-height"] = "テキストの高さを 1.5em, 150% などで指定します。";
oHelps["font-family"] = "フォントを指定します。カンマ（,）で複数記述すると、指定したフォントが無ければ次のフォントが採用されます。";
oHelps["text-indent"] = "テキストの最初の 1行のインデント（字下げ）を 1em のような単位付きの数値、10% のようなパーセント、inherit（継承）のいずれかで指定します。";
oHelps["text-align"] = "テキストの配置を left（規定値：左寄せ）、right（右寄せ）、center（センタリング）、justify（両端揃え）、inherit（継承）のいずれかで指定します。";
oHelps["text-justify"] = "単語間や文字間のスペースを調整して、文章の表示上の右端あわせを行います。justify には、auto（規定値）、newspaper（英文向け）、inter-word（単語間のみによる調整）、distribute-all-lines（最後の行も調整）、inter-ideograph（単語間と文字間による調整）、inter-cluster（アジア言語向け）、distribute（タイ言語向け）、kashida（アラビア語向け）などを指定します。";
oHelps["text-decoration"] = "テキストの装飾を none（何もなし）、underline（下線）、overline（上線）、line-through（打ち消し線）、blink（点滅）、inherit（継承）で指定します。複数指定も可能です。";
oHelps["text-underline-position"] = "下線の場所を、below（規定値：下側）、above（上側）のいずれかで指定します。";
oHelps["letter-spacing"] = "各文字間のスペースを normal（規定値）、2px などの長さ、inherit（継承）のいずれかで指定します。";
oHelps["word-spacing"] = "各ワード間のスペースを normal（規定値）、10px などの長さ、inherit（継承）のいずれかで指定します。";
oHelps["text-transform"] = "大文字、小文字変換について、capitalize（各単語の最初の文字を大文字にする）、uppercase（すべて大文字にする）、lowercase（すべて小文字にする）、none（規定値：何もしない）、inherit（継承）のいずれかで指定します。";
oHelps["white-space"] = "要素の中の空白がどのように扱われるかを normal（規定値：通常）、pre（自動改行しない、空白そのまま）、nowrap（自動改行しない、空白はつめる）、inherit（継承）のいずれかで指定します。";
oHelps["line-break"] = "行末の禁則処理を、normal（規定値：通常通り「。」や「、」が禁則処理される）、strict（「っ」や「ゃ」なども禁則処理される）のいずれかで指定します。";
oHelps["word-break"] = "行末の単語禁則処理を、normal（規定値：英文のみ単語の切れ目で改行）、break-all（英文、和文共に単語の途中でも改行）、keep-all（英文、和文共に単語の切れ目で改行）のいずれかで指定します。";
oHelps["ruby-align"] = "<ruby> 要素に対してルビの配置を、auto（規定値：自動）、left（左寄）、center（中央）、right（右寄）、distribute-letter（両端揃え）、distribute-space（均割）、line-edge（行末での処理変更）のいずれかで指定します。";
oHelps["ruby-overhang"] = "ルビが長い場合のはみ出し具合を auto（規定値：はみ出す）、whitespace（スペース文字分までははみ出す）、none（はみ出さない）のいずれかで指定します。";
oHelps["ruby-position"] = "<ruby> 要素に対してルビの配置を、above（規定値：上部）、inline（横）のいずれかで指定します。inline を指定した場合は、<ruby> 未対応のブラウザのように表示されます。";
oHelps["layout-grid"] = "行間や文字間のスペースを調整します。";
oHelps["layout-grid-line"] = "行間のスペースを none（規定値：指定無し）、auto（自動）、3px や 1em などの長さや割合（%）のいずれかで指定します。";
oHelps["layout-grid-char"] = "文字間のスペースを none（規定値：指定無し）、auto（自動）、3px や 1em などの長さや割合（%）のいずれかで指定します。";
oHelps["layout-grid-mode"] = "mode には both（規定値：line と char の両方を適用する）、none（どちらも適用しない）、line（line のみを適用する）、char（char のみを適用する）のいずれかを指定します。";
oHelps["layout-grid-type"] = "type には、loose（規定値：日本語や韓国語に適した方法）、strict（中国語にも適した方法）、fixed（固定）のいずれかを指定します。";
oHelps["text-autospace"] = "漢字のような表意文字と非表意文字の間にほんの少しだけ隙間を入れるかどうかを、none（規定値：隙間を入れない）、ideograph-alpha（表意文字と非表意文字の間に入れる）、ideograph-numeric（表意文字と数字の間に入れる）、ideograph-parenthesis（表意文字と括弧の間に入れる）、ideograph-space（表意文字の位置に適合するように入れる）のいずれかで指定します。";
oHelps["text-kashida-space"] = "アラビア語などのテキスト配置を最善化する際に用います。kashida には 50% のようなパーセントか、inherit（継承）のいずれかを指定します。";
oHelps["writing-mode"] = "縦書きを実現します。mode には lr-tb（横書き：left to right, top to bottom）、tb-rl（縦書き：top to bottom, right to left）のいずれかを指定します。";
oHelps["caption-side"] = "テーブルのキャプションの位置を top（上）、bottom（下）、right（右）、left（左）、inherit（継承）のいずれかで指定します。Netscape 6 では top と bottom のみ有効です。";
oHelps["table-layout"] = "テーブルのレイアウト方法を auto（規定値：自動）、fixed（固定）、inherit（継承）のいずれかで指定します。fixed を指定すると、テーブルの横幅をテーブルの内容に関わらず固定することができます。";
oHelps["border-collapse"] = "テーブルの各セルの枠線を collapse（隣のセルの枠線と重ねて表示）、separate（隣のセルとすこし離して表示）、inherit（継承）のいずれかで指定します。";
oHelps["border"] = "上下左右すべてのボーダー（枠線）の width, style, color を一度に指定します。";
oHelps["border-color"] = "上下左右すべてのボーダー（枠線）の色を red のような色の名前か、#ff0000 のような RGB形式などで指定します。";
oHelps["border-width"] = "上下左右すべてのボーダー（枠線）の太さを、thin（細線）、medium（規定値：中太線）、thick（太線）のいずれか、または 1px（幅指定） などのような単位付きの数値で指定します。";
oHelps["border-style"] = "上下左右すべてのボーダー（枠線）のスタイルを、none（線無し）、dotted（点線）、dashed（粗い点線）、solid（実線）、double（二重線）、groove（谷線）、ridge（山線）、inset（内線）、outset（外線）、inherit（継承）のいずれかで指定します。";
oHelps["border-top"] = "上のボーダー（枠線）の width, style, color を一度に指定します。";
oHelps["border-top-color"] = "上のボーダー（枠線）の色を red のような色の名前か、#ff0000 のような RGB形式などで指定します。";
oHelps["border-top-width"] = "上のボーダー（枠線）の太さを、thin（細線）、medium（規定値：中太線）、thick（太線）のいずれか、または 1px（幅指定） などのような単位付きの数値で指定します。";
oHelps["border-top-style"] = "上のボーダー（枠線）のスタイルを、none（線無し）、dotted（点線）、dashed（粗い点線）、solid（実線）、double（二重線）、groove（谷線）、ridge（山線）、inset（内線）、outset（外線）、inherit（継承）のいずれかで指定します。";
oHelps["border-left"] = "左のボーダー（枠線）の width, style, color を一度に指定します。";
oHelps["border-left-color"] = "左のボーダー（枠線）の色を red のような色の名前か、#ff0000 のような RGB形式などで指定します。";
oHelps["border-left-width"] = "左のボーダー（枠線）の太さを、thin（細線）、medium（規定値：中太線）、thick（太線）のいずれか、または 1px（幅指定） などのような単位付きの数値で指定します。";
oHelps["border-left-style"] = "左のボーダー（枠線）のスタイルを、none（線無し）、dotted（点線）、dashed（粗い点線）、solid（実線）、double（二重線）、groove（谷線）、ridge（山線）、inset（内線）、outset（外線）、inherit（継承）のいずれかで指定します。";
oHelps["border-right"] = "右のボーダー（枠線）の width, style, color を一度に指定します。";
oHelps["border-right-color"] = "右のボーダー（枠線）の色を red のような色の名前か、#ff0000 のような RGB形式などで指定します。";
oHelps["border-right-width"] = "右のボーダー（枠線）の太さを、thin（細線）、medium（規定値：中太線）、thick（太線）のいずれか、または 1px（幅指定） などのような単位付きの数値で指定します。";
oHelps["border-right-style"] = "右のボーダー（枠線）のスタイルを、none（線無し）、dotted（点線）、dashed（粗い点線）、solid（実線）、double（二重線）、groove（谷線）、ridge（山線）、inset（内線）、outset（外線）、inherit（継承）のいずれかで指定します。";
oHelps["border-bottom"] = "下のボーダー（枠線）の width, style, color を一度に指定します。";
oHelps["border-bottom-color"] = "下のボーダー（枠線）の色を red のような色の名前か、#ff0000 のような RGB形式などで指定します。";
oHelps["border-bottom-width"] = "下のボーダー（枠線）の太さを、thin（細線）、medium（規定値：中太線）、thick（太線）のいずれか、または 1px（幅指定） などのような単位付きの数値で指定します。";
oHelps["border-bottom-style"] = "下のボーダー（枠線）のスタイルを、none（線無し）、dotted（点線）、dashed（粗い点線）、solid（実線）、double（二重線）、groove（谷線）、ridge（山線）、inset（内線）、outset（外線）、inherit（継承）のいずれかで指定します。";
oHelps["margin"] = "上下左右のマージン（余白）を 20px, 3em、10% などのように指定します。";
oHelps["margin-top"] = "上のマージン（余白）を 20px, 3em、10% などのように指定します。";
oHelps["margin-left"] = "左のマージン（余白）を 20px, 3em、10% などのように指定します。";
oHelps["margin-right"] = "右のマージン（余白）を 20px, 3em、10% などのように指定します。";
oHelps["margin-bottom"] = "下のマージン（余白）を 20px, 3em、10% などのように指定します。";
oHelps["padding"] = "上下左右のパディング（余白）を 2.0em, 2em, 120% などのように指定します。";
oHelps["padding-top"] = "上のパディング（余白）を 2.0em, 2em, 120% などのように指定します。";
oHelps["padding-left"] = "左のパディング（余白）を 2.0em, 2em, 120% などのように指定します。";
oHelps["padding-right"] = "右のパディング（余白）を 2.0em, 2em, 120% などのように指定します。";
oHelps["padding-bottom"] = "下のパディング（余白）を 2.0em, 2em, 120% などのように指定します。";

function DispHelp(func)
{
	return overlib(oHelps[func], CAPTION, func)
}

function GetE( elementId )
{
	return document.getElementById( elementId )  ;
}

function ShowE( element, isVisible )
{
	if ( typeof( element ) == 'string' )
		element = GetE( element ) ;
	element.style.display = isVisible ? '' : 'none' ;
}

function SetSelectedTab( tabCode )
{
	for ( var sCode in oTabs ) {
		if ( sCode == tabCode )
			oTabs[sCode].className = 'PopupTabSelected' ;
		else
			oTabs[sCode].className = 'PopupTab' ;
	}
	ShowE("divGeneral", (tabCode == "General"));
	ShowE("divList", (tabCode == "List"));
	ShowE("divPrint", (tabCode == "Print"));
	ShowE("divBackground", (tabCode == "Background"));
	ShowE("divFont", (tabCode == "Font"));
	ShowE("divText", (tabCode == "Text"));
	ShowE("divTable", (tabCode == "Table"));
	ShowE("divBorder", (tabCode == "Border"));
	ShowE("divMargin", (tabCode == "Margin"));
	ShowE("divPadding", (tabCode == "Padding"));
}

function TabDiv_OnClick()
{
	SetSelectedTab( this.TabCode ) ;
}

function AddTab(tabCode, tabText)
{
	if ( typeof( oTabs[ tabCode ] ) != 'undefined' )
		return ;

	var eTabsRow = document.getElementById( 'Tabs' ) ;

	var oCell = eTabsRow.insertCell(  eTabsRow.cells.length - 1 ) ;
	oCell.noWrap = true ;

	var oDiv = document.createElement( 'DIV' ) ;
	oDiv.className = 'PopupTab' ;
	oDiv.innerHTML = tabText ;
	oDiv.TabCode = tabCode ;
	oDiv.onclick = TabDiv_OnClick ;

	eTabsRow = document.getElementById( 'TabsRow' ) ;

	oCell.appendChild( oDiv ) ;

	if ( eTabsRow.style.display == 'none' ) {
		oDiv.className = 'PopupTabSelected' ;
		eTabsRow.style.display = '' ;
	//	window.onresize() ;
	}

	oTabs[ tabCode ] = oDiv ;
	oTabs[ tabCode ].Index = oTabs.length - 1 ;
}

function Init()
{
	var aParams = document.location.search.substr(1).split('&') ;
	for ( var i = 0 ; i < aParams.length ; i++ ) {
		var aParam = aParams[i].split('=') ;
		var sParamName  = aParam[0] ;
		var sParamValue = aParam[1] ;
		if (sParamName == "action") {
			frm2.action = sParamValue;
		}
	}
	AddTab("General", "全体");
	AddTab("List", "リスト");
	AddTab("Print", "印刷");
	AddTab("Background", "背景");
	AddTab("Font", "フォント");
	AddTab("Text", "テキスト");
	AddTab("Table", "テーブル");
	AddTab("Border", "ボーダー");
	AddTab("Margin", "マージン");
	AddTab("Padding", "パディング");
	SetSelectedTab("General");
}

function redmarkon()
{
	CSSSel=document.getElementsByName("SelectorName")[0];
	if(CSSSel.Value != "none"){
		rules = window.opener.document.styleSheets[SyNum].cssRules  || window.opener.document.styleSheets[SyNum].rules;
		var CurStyle = rules[CSSSel.value].style;
		xborderTopColor=CurStyle.borderTopColor;
		xborderTopStyle=CurStyle.borderTopStyle;
		xborderTopWidth=CurStyle.borderTopWidth;
		xborderLeftColor=CurStyle.borderLeftColor;
		xborderLeftStyle=CurStyle.borderLeftStyle;
		xborderLeftWidth=CurStyle.borderLeftWidth;
		xborderRightColor=CurStyle.borderRightColor;
		xborderRightStyle=CurStyle.borderRightStyle;
		xborderRightWidth=CurStyle.borderRightWidth;
		xborderBottomColor=CurStyle.borderBottomColor;
		xborderBottomStyle=CurStyle.borderBottomStyle;
		xborderBottomWidth=CurStyle.borderBottomWidth;
		CurStyle.border="3px solid red";
	}
}

function redmarkoff()
{
	CSSSel=document.getElementsByName("SelectorName")[0];
	if (CSSSel.Value != "none") {
		rules = window.opener.document.styleSheets[SyNum].cssRules  || window.opener.document.styleSheets[SyNum].rules;
		var CurStyle = rules[CSSSel.value].style;
		CurStyle.borderTopColor=xborderTopColor;
		CurStyle.borderTopStyle=xborderTopStyle;
		CurStyle.borderTopWidth=xborderTopWidth;
		CurStyle.borderLeftColor=xborderLeftColor;
		CurStyle.borderLeftStyle=xborderLeftStyle;
		CurStyle.borderLeftWidth=xborderLeftWidth;
		CurStyle.borderRightColor=xborderRightColor;
		CurStyle.borderRightStyle=xborderRightStyle;
		CurStyle.borderRightWidth=xborderRightWidth;
		CurStyle.borderBottomColor=xborderBottomColor;
		CurStyle.borderBottomStyle=xborderBottomStyle;
		CurStyle.borderBottomWidth=xborderBottomWidth;
	}
}

function SelectCombo(element, val)
{
	element.options[0].selected = true;
	if (typeof(val) != "undefined") {
		for (i=0; i < element.options.length ;i++) {
			if (element.options[i].value == val) {
				element.options[0].selected = false;
				element.options[i].selected = true;
				break;
			}
		}
	}
}

function SelectorChange()
{
	CSSSel=document.getElementsByName("SelectorName")[0];
	if (CSSSel.value != "none") {
		rules = window.opener.document.styleSheets[SyNum].cssRules  || window.opener.document.styleSheets[SyNum].rules;
		var CurStyle = rules[CSSSel.value].style;
		Strings = CurStyle.cssText;
		Strings = Strings.replace(/;/g,";<br>\n\t");
		document.getElementById("SelCode").innerHTML=Strings;
		// 全体
		SelectCombo(document.getElementsByName("display")[0], CurStyle.display);
		SelectCombo(document.getElementsByName("position")[0], CurStyle.position);
		document.getElementsByName("top")[0].value = CurStyle.top;
		document.getElementsByName("left")[0].value = CurStyle.left;
		document.getElementsByName("right")[0].value = CurStyle.right;
		document.getElementsByName("bottom")[0].value = CurStyle.bottom;
		if (typeof(CurStyle.styleFloat) != "undefined") {
//			SelectCombo(document.getElementsByName("styleFloat")[0], CurStyle.styleFloat);
			SelectCombo(document.getElementsByName("float")[0], CurStyle.styleFloat);
		}
		if (typeof(CurStyle.cssFloat) != "undefined"){ 
//			SelectCombo(document.getElementsByName("styleFloat")[0], CurStyle.cssFloat);
			SelectCombo(document.getElementsByName("float")[0], CurStyle.cssFloat);
		}
		SelectCombo(document.getElementsByName("clear")[0], CurStyle.clear);
		document.getElementsByName("zIndex")[0].value = CurStyle.zIndex;
		SelectCombo(document.getElementsByName("direction")[0], CurStyle.direction);
		SelectCombo(document.getElementsByName("unicodeBidi")[0], CurStyle.unicodeBidi);
		document.getElementsByName("width")[0].value = CurStyle.width;
		document.getElementsByName("height")[0].value = CurStyle.height;
		document.getElementsByName("verticalAlign")[0].value = CurStyle.verticalAlign;
		SelectCombo(document.getElementsByName("overflow")[0], CurStyle.overflow);
		SelectCombo(document.getElementsByName("overflowX")[0], CurStyle.overflowX);
		SelectCombo(document.getElementsByName("overflowY")[0], CurStyle.overflowY);
		document.getElementsByName("clip")[0].value = CurStyle.clip;
		SelectCombo(document.getElementsByName("visibility")[0], CurStyle.visibility);
		SelectCombo(document.getElementsByName("cursor")[0], CurStyle.cursor);
		SelectCombo(document.getElementsByName("imeMode")[0], CurStyle.imeMode);
		document.getElementsByName("behavior")[0].value = CurStyle.behavior;
		document.getElementsByName("filter")[0].value = CurStyle.filter;
		document.getElementsByName("scrollbarBaseColor")[0].value = CurStyle.scrollbarBaseColor;
		document.getElementsByName("scrollbarTrackColor")[0].value = CurStyle.scrollbarTrackColor;
		document.getElementsByName("scrollbarFaceColor")[0].value = CurStyle.scrollbarFaceColor;
		document.getElementsByName("scrollbarShadowColor")[0].value = CurStyle.scrollbarShadowColor;
//		document.getElementsByName("scrollbarDarkshadowColor")[0].value = CurStyle.scrollbarDarkshadowColor;
		document.getElementsByName("scrollbarHighlightColor")[0].value = CurStyle.scrollbarHighlightColor;
		document.getElementsByName("scrollbar3dlightColor")[0].value = CurStyle.scrollbar3dLightColor;
		document.getElementsByName("scrollbarArrowColor")[0].value = CurStyle.scrollbarArrowColor;

		// マージン
		document.getElementsByName("margin")[0].value=CurStyle.margin;
		document.getElementsByName("marginBottom")[0].value=CurStyle.marginBottom;
		document.getElementsByName("marginLeft")[0].value=CurStyle.marginLeft;
		document.getElementsByName("marginRight")[0].value=CurStyle.marginRight;
		document.getElementsByName("marginTop")[0].value=CurStyle.marginTop;

		// パディング
		document.getElementsByName("padding")[0].value=CurStyle.padding;
		document.getElementsByName("paddingBottom")[0].value=CurStyle.paddingBottom;
		document.getElementsByName("paddingLeft")[0].value=CurStyle.paddingLeft;
		document.getElementsByName("paddingRight")[0].value=CurStyle.paddingRight;
		document.getElementsByName("paddingTop")[0].value=CurStyle.paddingTop;

		//  ボーダー
		document.getElementsByName("border")[0].value=CurStyle.border;
		document.getElementsByName("borderColor")[0].value=CurStyle.borderColor;
		document.getElementsByName("borderWidth")[0].value=CurStyle.borderWidth;
		document.getElementsByName("borderStyle")[0].value=CurStyle.borderStyle;
		document.getElementsByName("borderTop")[0].value=CurStyle.borderTop;
		document.getElementsByName("borderTopColor")[0].value=CurStyle.borderTopColor;
		SelectCombo(document.getElementsByName("borderTopStyle")[0], CurStyle.borderTopStyle);
		document.getElementsByName("borderTopWidth")[0].value=CurStyle.borderTopWidth;
		document.getElementsByName("borderLeft")[0].value=CurStyle.borderLeft;
		document.getElementsByName("borderLeftColor")[0].value=CurStyle.borderLeftColor;
		SelectCombo(document.getElementsByName("borderLeftStyle")[0], CurStyle.borderLeftStyle);
		document.getElementsByName("borderLeftWidth")[0].value=CurStyle.borderLeftWidth;
		document.getElementsByName("borderRight")[0].value=CurStyle.borderRight;
		document.getElementsByName("borderRightColor")[0].value=CurStyle.borderRightColor;
		SelectCombo(document.getElementsByName("borderRightStyle")[0], CurStyle.borderRightStyle);
		document.getElementsByName("borderRightWidth")[0].value=CurStyle.borderRightWidth;
		document.getElementsByName("borderBottom")[0].value=CurStyle.borderBottom;
		document.getElementsByName("borderBottomColor")[0].value=CurStyle.borderBottomColor;
		SelectCombo(document.getElementsByName("borderBottomStyle")[0], CurStyle.borderBottomStyle);
		document.getElementsByName("borderBottomWidth")[0].value=CurStyle.borderBottomWidth;

		//  リスト
		document.getElementsByName("listStyle")[0].value = CurStyle.listStyle;
		document.getElementsByName("listStyleImage")[0].value = CurStyle.listStyleImage;
		SelectCombo(document.getElementsByName("listStyleType")[0], CurStyle.listStyleType);
		SelectCombo(document.getElementsByName("listStylePosition")[0], CurStyle.listStyleType);

		//  印刷
		SelectCombo(document.getElementsByName("pageBreakBefore")[0], CurStyle.pageBreakBefore);
		SelectCombo(document.getElementsByName("pageBreakAfter")[0], CurStyle.pageBreakAfter);

		//  背景
		document.getElementsByName("background")[0].value = CurStyle.background;
		document.getElementsByName("backgroundColor")[0].value = CurStyle.backgroundColor;
		document.getElementsByName("backgroundImage")[0].value = CurStyle.backgroundImage;
		SelectCombo(document.getElementsByName("backgroundRepeat")[0], CurStyle.backgroundRepeat);
		SelectCombo(document.getElementsByName("backgroundAttachment")[0], CurStyle.backgroundAttachment);
		if (typeof(CurStyle.backgroundPositionX) == "undefined") {
			 if (CurStyle.backgroundPosition != "") {
				positionStrings= CurStyle.backgroundPosition;
				positionArr = positionStrings.split(" ",2);
				document.getElementsByName("backgroundPositionX")[0].value=positionArr[0];
				document.getElementsByName("backgroundPositionY")[0].value=positionArr[1];
			} else {
				document.getElementsByName("backgroundPositionX")[0].value="";
				document.getElementsByName("backgroundPositionY")[0].value="";
			}
		} else {
			document.getElementsByName("backgroundPositionX")[0].value=CurStyle.backgroundPositionX;
			document.getElementsByName("backgroundPositionY")[0].value=CurStyle.backgroundPositionY;
		}

		//  フォント
//		document.getElementsByName("fontAll")[0].value=CurStyle.font;
		document.getElementsByName("color")[0].value=CurStyle.color;
		SelectCombo(document.getElementsByName("fontStyle")[0], CurStyle.fontStyle);
		SelectCombo(document.getElementsByName("fontVariant")[0], CurStyle.fontVariant);
		SelectCombo(document.getElementsByName("fontWeight")[0], CurStyle.fontWeight);
		document.getElementsByName("fontSize")[0].value=CurStyle.fontSize;
		document.getElementsByName("lineHeight")[0].value=CurStyle.lineHeight;
		document.getElementsByName("fontFamily")[0].value=CurStyle.fontFamily;

		//  テキスト
		document.getElementsByName("textIndent")[0].value=CurStyle.textIndent;
		SelectCombo(document.getElementsByName("textAlign")[0], CurStyle.textAlign);
		SelectCombo(document.getElementsByName("textJustify")[0], CurStyle.textJustify);
		SelectCombo(document.getElementsByName("textDecoration")[0], CurStyle.textDecoration);
		SelectCombo(document.getElementsByName("textUnderlinePosition")[0], CurStyle.textUnderlinePosition);
		document.getElementsByName("letterSpacing")[0].value=CurStyle.letterSpacing;
		document.getElementsByName("wordSpacing")[0].value=CurStyle.wordSpacing;
		SelectCombo(document.getElementsByName("textTransform")[0], CurStyle.textTransform);
		SelectCombo(document.getElementsByName("whiteSpace")[0], CurStyle.whiteSpace);
		SelectCombo(document.getElementsByName("lineBreak")[0], CurStyle.lineBreak);
		SelectCombo(document.getElementsByName("wordBreak")[0], CurStyle.wordBreak);
		SelectCombo(document.getElementsByName("rubyAlign")[0], CurStyle.rubyAlign);
		SelectCombo(document.getElementsByName("rubyOverhang")[0], CurStyle.rubyOverhang);
		SelectCombo(document.getElementsByName("rubyPosition")[0], CurStyle.rubyPosition);
		document.getElementsByName("layoutGrid")[0].value=CurStyle.layoutGrid;
		document.getElementsByName("layoutGridLine")[0].value=CurStyle.layoutGridLine;
		document.getElementsByName("layoutGridChar")[0].value=CurStyle.layoutGridChar;
		SelectCombo(document.getElementsByName("layoutGridMode")[0], CurStyle.layoutGridMode);
		SelectCombo(document.getElementsByName("layoutGridType")[0], CurStyle.layoutGridType);
		SelectCombo(document.getElementsByName("textAutospace")[0], CurStyle.textAutospace);
		document.getElementsByName("textKashidaSpace")[0].value=CurStyle.textKashidaSpace;
		SelectCombo(document.getElementsByName("writingMode")[0], CurStyle.writingMode);

		//  テーブル
		SelectCombo(document.getElementsByName("captionSide")[0], CurStyle.captionSide);
		SelectCombo(document.getElementsByName("tableLayout")[0], CurStyle.tableLayout);
		SelectCombo(document.getElementsByName("borderCollapse")[0], CurStyle.borderCollapse);
	}
}

function squareborder()
{
	document.getElementsByName("borderLeftColor")[0].value = document.getElementsByName("borderTopColor")[0].value;
	document.getElementsByName("borderLeftStyle")[0].value = document.getElementsByName("borderTopStyle")[0].value;
	document.getElementsByName("borderLeftWidth")[0].value = document.getElementsByName("borderTopWidth")[0].value;
	document.getElementsByName("borderRightColor")[0].value = document.getElementsByName("borderTopColor")[0].value;
	document.getElementsByName("borderRightStyle")[0].value = document.getElementsByName("borderTopStyle")[0].value;
	document.getElementsByName("borderRightWidth")[0].value = document.getElementsByName("borderTopWidth")[0].value;
	document.getElementsByName("borderBottomColor")[0].value = document.getElementsByName("borderTopColor")[0].value;
	document.getElementsByName("borderBottomStyle")[0].value = document.getElementsByName("borderTopStyle")[0].value;
	document.getElementsByName("borderBottomWidth")[0].value = document.getElementsByName("borderTopWidth")[0].value;
}

function UpdateStyle(obj, val)
{
	if (typeof(obj) != "undefined") {
		obj = val;
	}
}

function changeupdate()
{
	CSSSel=document.getElementsByName("SelectorName")[0];
	if (CSSSel.value != "none") {
		rules = window.opener.document.styleSheets[SyNum].cssRules  || window.opener.document.styleSheets[SyNum].rules;
		var CurStyle = rules[CSSSel.value].style;

		// 全体
		if (typeof(CurStyle.display) != "undefined") CurStyle.display = document.getElementsByName("display")[0].value;
		if (typeof(CurStyle.position) != "undefined") CurStyle.position = document.getElementsByName("position")[0].value;
		if (typeof(CurStyle.top) != "undefined") CurStyle.top = document.getElementsByName("top")[0].value;
		if (typeof(CurStyle.left) != "undefined") CurStyle.left = document.getElementsByName("left")[0].value;
		if (typeof(CurStyle.right) != "undefined") CurStyle.right = document.getElementsByName("right")[0].value;
		if (typeof(CurStyle.bottom) != "undefined") CurStyle.bottom = document.getElementsByName("bottom")[0].value;
		if (typeof(CurStyle.styleFloat) != "undefined") CurStyle.styleFloat = document.getElementsByName("styleFloat")[0].value;
		if (typeof(CurStyle.cssFloat) != "undefined") CurStyle.cssFloat = document.getElementsByName("styleFloat")[0].value;
		if (typeof(CurStyle.clear) != "undefined") CurStyle.clear = document.getElementsByName("clear")[0].value;
		if (typeof(CurStyle.zIndex) != "undefined") CurStyle.zIndex = document.getElementsByName("zIndex")[0].value;
		if (typeof(CurStyle.direction) != "undefined") CurStyle.direction = document.getElementsByName("direction")[0].value;
		if (typeof(CurStyle.unicodeBidi) != "undefined") CurStyle.unicodeBidi = document.getElementsByName("unicodeBidi")[0].value;
		if (typeof(CurStyle.width) != "undefined") CurStyle.width = document.getElementsByName("width")[0].value;
		if (typeof(CurStyle.height) != "undefined") CurStyle.height = document.getElementsByName("height")[0].value;
		if (typeof(CurStyle.verticalAlign) != "undefined") CurStyle.verticalAlign = document.getElementsByName("verticalAlign")[0].value;
		if (typeof(CurStyle.overflow) != "undefined") CurStyle.overflow = document.getElementsByName("overflow")[0].value;
		if (typeof(CurStyle.overflowX) != "undefined") CurStyle.overflowX = document.getElementsByName("overflowX")[0].value;
		if (typeof(CurStyle.overflowY) != "undefined") CurStyle.overflowY = document.getElementsByName("overflowY")[0].value;
		if (typeof(CurStyle.clip) != "undefined") CurStyle.clip = document.getElementsByName("clip")[0].value;
		if (typeof(CurStyle.visibility) != "undefined") CurStyle.visibility = document.getElementsByName("visibility")[0].value;
		if (typeof(CurStyle.cursor) != "undefined") CurStyle.cursor = document.getElementsByName("cursor")[0].value;
		if (typeof(CurStyle.imeMode) != "undefined") CurStyle.imeMode = document.getElementsByName("imeMode")[0].value;
		if (typeof(CurStyle.behavior) != "undefined") CurStyle.behavior = document.getElementsByName("behavior")[0].value;
		if (typeof(CurStyle.filter) != "undefined") CurStyle.filter = document.getElementsByName("filter")[0].value;
		if (typeof(CurStyle.scrollbarBaseColor) != "undefined") CurStyle.scrollbarBaseColor = document.getElementsByName("scrollbarBaseColor")[0].value;
		if (typeof(CurStyle.scrollbarTrackColor) != "undefined") CurStyle.scrollbarTrackColor = document.getElementsByName("scrollbarTrackColor")[0].value;
		if (typeof(CurStyle.scrollbarFaseColor) != "undefined") CurStyle.scrollbarFaseColor = document.getElementsByName("scrollbarFaseColor")[0].value;
		if (typeof(CurStyle.scrollbarShadowColor) != "undefined") CurStyle.scrollbarShadowColor = document.getElementsByName("scrollbarShadowColor")[0].value;
//		if (typeof(CurStyle.scrollbarDarkshadowColor) != "undefined") CurStyle.scrollbarDarkshadowColor = document.getElementsByName("scrollbarDarkshadowColor")[0].value;
		if (typeof(CurStyle.scrollbarHighlightColor) != "undefined") CurStyle.scrollbarHighlightColor = document.getElementsByName("scrollbarHighlightColor")[0].value;
		if (typeof(CurStyle.scrollbar3dLightColor) != "undefined") CurStyle.scrollbar3dLightColor = document.getElementsByName("scrollbar3dlightColor")[0].value;
		if (typeof(CurStyle.scrollbarArrowColor) != "undefined") CurStyle.scrollbarArrowColor = document.getElementsByName("scrollbarArrowColor")[0].value;

		// マージン
		if (typeof(CurStyle.margin) != "undefined") CurStyle.margin = document.getElementsByName("margin")[0].value;
		if (typeof(CurStyle.marginBottom) != "undefined") CurStyle.marginBottom = document.getElementsByName("marginBottom")[0].value;
		if (typeof(CurStyle.marginLeft) != "undefined") CurStyle.marginLeft = document.getElementsByName("marginLeft")[0].value;
		if (typeof(CurStyle.marginRight) != "undefined") CurStyle.marginRight = document.getElementsByName("marginRight")[0].value;
		if (typeof(CurStyle.marginTop) != "undefined") CurStyle.marginTop = document.getElementsByName("marginTop")[0].value;

		// パディング
		if (typeof(CurStyle.padding) != "undefined") CurStyle.padding = document.getElementsByName("padding")[0].value;
		if (typeof(CurStyle.paddingBottom) != "undefined") CurStyle.paddingBottom = document.getElementsByName("paddingBottom")[0].value;
		if (typeof(CurStyle.paddingLeft) != "undefined") CurStyle.paddingLeft = document.getElementsByName("paddingLeft")[0].value;
		if (typeof(CurStyle.paddingRight) != "undefined") CurStyle.paddingRight = document.getElementsByName("paddingRight")[0].value;
		if (typeof(CurStyle.paddingTop) != "undefined") CurStyle.paddingTop = document.getElementsByName("paddingTop")[0].value;

		// ボーダー
		if (typeof(CurStyle.border) != "undefined") CurStyle.border = document.getElementsByName("border")[0].value;
		if (typeof(CurStyle.borderColor) != "undefined") CurStyle.borderColor = document.getElementsByName("borderColor")[0].value;
		if (typeof(CurStyle.borderStyle) != "undefined") CurStyle.borderStyle = document.getElementsByName("borderStyle")[0].value;
		if (typeof(CurStyle.borderWidth) != "undefined") CurStyle.borderWidth = document.getElementsByName("borderWidth")[0].value;
		if (typeof(CurStyle.borderTop) != "undefined") CurStyle.borderTop = document.getElementsByName("borderTop")[0].value;
		if (typeof(CurStyle.borderTopColor) != "undefined") CurStyle.borderTopColor = document.getElementsByName("borderTopColor")[0].value;
		if (typeof(CurStyle.borderTopStyle) != "undefined") CurStyle.borderTopStyle = document.getElementsByName("borderTopStyle")[0].value;
		if (typeof(CurStyle.borderTopWidth) != "undefined") CurStyle.borderTopWidth = document.getElementsByName("borderTopWidth")[0].value;
		if (typeof(CurStyle.borderLeft) != "undefined") CurStyle.borderLeft = document.getElementsByName("borderLeft")[0].value;
		if (typeof(CurStyle.borderLeftColor) != "undefined") CurStyle.borderLeftColor = document.getElementsByName("borderLeftColor")[0].value;
		if (typeof(CurStyle.borderLeftStyle) != "undefined") CurStyle.borderLeftStyle = document.getElementsByName("borderLeftStyle")[0].value;
		if (typeof(CurStyle.borderLeftWidth) != "undefined") CurStyle.borderLeftWidth = document.getElementsByName("borderLeftWidth")[0].value;
		if (typeof(CurStyle.borderRight) != "undefined") CurStyle.borderRight = document.getElementsByName("borderRight")[0].value;
		if (typeof(CurStyle.borderRightColor) != "undefined") CurStyle.borderRightColor = document.getElementsByName("borderRightColor")[0].value;
		if (typeof(CurStyle.borderRightStyle) != "undefined") CurStyle.borderRightStyle = document.getElementsByName("borderRightStyle")[0].value;
		if (typeof(CurStyle.borderRightWidth) != "undefined") CurStyle.borderRightWidth = document.getElementsByName("borderRightWidth")[0].value;
		if (typeof(CurStyle.borderBottom) != "undefined") CurStyle.borderBottom = document.getElementsByName("borderBottom")[0].value;
		if (typeof(CurStyle.borderBottomColor) != "undefined") CurStyle.borderBottomColor = document.getElementsByName("borderBottomColor")[0].value;
		if (typeof(CurStyle.borderBottomStyle) != "undefined") CurStyle.borderBottomStyle = document.getElementsByName("borderBottomStyle")[0].value;
		if (typeof(CurStyle.borderBottomWidth) != "undefined") CurStyle.borderBottomWidth = document.getElementsByName("borderBottomWidth")[0].value;

		//  リスト
		if (typeof(CurStyle.listStyle) != "undefined") CurStyle.listStyle = document.getElementsByName("listStyle")[0].value;
		if (typeof(CurStyle.listStyleImage) != "undefined") CurStyle.listStyleImage = document.getElementsByName("listStyleImage")[0].value;
		if (typeof(CurStyle.listStyleType) != "undefined") CurStyle.listStyleType = document.getElementsByName("listStyleType")[0].value;
		if (typeof(CurStyle.listStyleType) != "undefined") CurStyle.listStyleType = document.getElementsByName("listStylePosition")[0].value;

		//  印刷
		if (typeof(CurStyle.pageBreakBefore) != "undefined") CurStyle.pageBreakBefore = document.getElementsByName("pageBreakBefore")[0].value;
		if (typeof(CurStyle.pageBreakAfter) != "undefined") CurStyle.pageBreakAfter = document.getElementsByName("pageBreakAfter")[0].value;

		// 背景
		if (typeof(CurStyle.background) != "undefined") CurStyle.background = document.getElementsByName("background")[0].value;
		if (typeof(CurStyle.backgroundColor) != "undefined") CurStyle.backgroundColor = document.getElementsByName("backgroundColor")[0].value;
		if (typeof(CurStyle.backgroundImage) != "undefined") CurStyle.backgroundImage = document.getElementsByName("backgroundImage")[0].value;
		if (typeof(CurStyle.backgroundRepeat) != "undefined") CurStyle.backgroundRepeat = document.getElementsByName("backgroundRepeat")[0].value;
		if (typeof(CurStyle.backgroundAttachment) != "undefined") CurStyle.backgroundAttachment = document.getElementsByName("backgroundAttachment")[0].value;
		if (typeof(CurStyle.backgroundPositionX) == "undefined") {
			positionStrings= document.getElementsByName("backgroundPositionX")[0].value;
			positionStrings=positionStrings.concat(" ");
			positionStrings=positionStrings.concat(document.getElementsByName("backgroundPositionY")[0].value);
			if (typeof(CurStyle.backgroundPosition) != "undefined") CurStyle.backgroundPosition = positionStrings;
		} else {
			if (typeof(CurStyle.backgroundPositionX) != "undefined") CurStyle.backgroundPositionX = document.getElementsByName("backgroundPositionX")[0].value;
			if (typeof(CurStyle.backgroundPositionY) != "undefined") CurStyle.backgroundPositionY = document.getElementsByName("backgroundPositionY")[0].value;
		}

		// フォント
//		if (typeof(CurStyle.font) != "undefined") CurStyle.font = document.getElementsByName("fontAll")[0].value;
		if (typeof(CurStyle.color) != "undefined") CurStyle.color = document.getElementsByName("color")[0].value;
		if (typeof(CurStyle.fontStyle) != "undefined") CurStyle.fontStyle = document.getElementsByName("fontStyle")[0].value;
		if (typeof(CurStyle.fontVariant) != "undefined") CurStyle.fontVariant = document.getElementsByName("fontVariant")[0].value;
		if (typeof(CurStyle.fontWeight) != "undefined") CurStyle.fontWeight = document.getElementsByName("fontWeight")[0].value;
		if (typeof(CurStyle.fontSize) != "undefined") CurStyle.fontSize = document.getElementsByName("fontSize")[0].value;
		if (typeof(CurStyle.fontHeight) != "undefined") CurStyle.fontHeight = document.getElementsByName("fontHeight")[0].value;
		if (typeof(CurStyle.fontFamily) != "undefined") CurStyle.fontFamily = document.getElementsByName("fontFamily")[0].value;

		// テキスト
		if (typeof(CurStyle.textIndent) != "undefined") CurStyle.textIndent = document.getElementsByName("textIndent")[0].value;
		if (typeof(CurStyle.textAlign) != "undefined") CurStyle.textAlign = document.getElementsByName("textAlign")[0].value;
		if (typeof(CurStyle.textJustify) != "undefined") CurStyle.textJustify = document.getElementsByName("textJustify")[0].value;
		if (typeof(CurStyle.textAlign) != "undefined") CurStyle.textDecoration = document.getElementsByName("textDecoration")[0].value;
		if (typeof(CurStyle.textUnderlinePosition) != "undefined") CurStyle.textUnderlinePosition = document.getElementsByName("textUnderlinePosition")[0].value;
		if (typeof(CurStyle.letterSpacing) != "undefined") CurStyle.letterSpacing = document.getElementsByName("letterSpacing")[0].value;
		if (typeof(CurStyle.wordSpacing) != "undefined") CurStyle.wordSpacing = document.getElementsByName("wordSpacing")[0].value;
		if (typeof(CurStyle.textTransform) != "undefined") CurStyle.textTransform = document.getElementsByName("textTransform")[0].value;
		if (typeof(CurStyle.whiteSpace) != "undefined") CurStyle.whiteSpace = document.getElementsByName("whiteSpace")[0].value;
		if (typeof(CurStyle.lineBreak) != "undefined") CurStyle.lineBreak = document.getElementsByName("lineBreak")[0].value;
		if (typeof(CurStyle.wordBreak) != "undefined") CurStyle.wordBreak = document.getElementsByName("wordBreak")[0].value;
		if (typeof(CurStyle.rubyAlign) != "undefined") CurStyle.rubyAlign = document.getElementsByName("rubyAlign")[0].value;
		if (typeof(CurStyle.rubyOverhang) != "undefined") CurStyle.rubyOverhang = document.getElementsByName("rubyOverhang")[0].value;
		if (typeof(CurStyle.rubyPosition) != "undefined") CurStyle.rubyPosition = document.getElementsByName("rubyPosition")[0].value;
		if (typeof(CurStyle.layoutGrid) != "undefined") CurStyle.layoutGrid = document.getElementsByName("layoutGrid")[0].value;
		if (typeof(CurStyle.layoutGridLine) != "undefined") CurStyle.layoutGridLine = document.getElementsByName("layoutGridLine")[0].value;
		if (typeof(CurStyle.layoutGridChar) != "undefined") CurStyle.layoutGridChar = document.getElementsByName("layoutGridChar")[0].value;
		if (typeof(CurStyle.layoutGridMode) != "undefined") CurStyle.layoutGridMode = document.getElementsByName("layoutGridMode")[0].value;
		if (typeof(CurStyle.layoutGridType) != "undefined") CurStyle.layoutGridType = document.getElementsByName("layoutGridType")[0].value;
		if (typeof(CurStyle.textAutospace) != "undefined") CurStyle.textAutospace = document.getElementsByName("textAutospace")[0].value;
		if (typeof(CurStyle.textKashidaSpace) != "undefined") CurStyle.textKashidaSpace = document.getElementsByName("textKashidaSpace")[0].value;
		if (typeof(CurStyle.writingMode) != "undefined") CurStyle.writingMode = document.getElementsByName("writingMode")[0].value;

		//  テーブル
		if (typeof(CurStyle.captionSide) != "undefined") CurStyle.captionSide = document.getElementsByName("captionSide")[0].value;
		if (typeof(CurStyle.tableLayout) != "undefined") CurStyle.tableLayout = document.getElementsByName("tableLayout")[0].value;
		if (typeof(CurStyle.borderCollapse) != "undefined") CurStyle.borderCollapse = document.getElementsByName("borderCollapse")[0].value;

		Strings ="";
		if (typeof(window.opener.document.styleSheets[SyNum].cssText) != "undefined") {
			Strings=window.opener.document.styleSheets[SyNum].cssText;
		} else {
			rules = window.opener.document.styleSheets[SyNum].cssRules || window.opener.document.styleSheets[SyNum].rules; 
			for (i=0; i < rules.length ;i++) {
				Strings=Strings.concat(rules[i].cssText);
				Strings=Strings.concat("\n");
		 	}
		}
		if (typeof(document.getElementById("SrcCode").innerText)!= "undefined") {
			document.getElementById("SrcCode").innerText=Strings;
		} else {
			document.getElementById("SrcCode").innerHTML=Strings;
		}
	}
}/*end of function */

function PaletOpen(paletname)
{
	tdobj=document.getElementById(paletname).parentNode || document.getElementById(paletname).parentElement;
	trobj=tdobj.parentNode || tdobj.parentElement;

	if(trobj.style.display == "none"){
		trobj.style.display="";
	} else {
		trobj.style.display="none";	
	}
}/*end of function */

function paletclick(e,p)
{
	if (typeof(e.target) != "undefined") {
		document.getElementsByName(p)[0].value=e.target.getAttribute("bgcolor");
	}
	if(typeof(window.event.srcElement) != "undefined") {
		document.getElementsByName(p)[0].value=window.event.srcElement.getAttribute("bgcolor");
	}
}

/*
function jumpsrc() {
	reviseSrc();
	document.getElementById("SrcCode").focus();
	document.getElementById("SrcCode").select();
}
*/

function changeupdatewrite()
{
	reviseSrc();
	changeupdate();
}

function reviseSrc()
{
	Strings = document.getElementById("SrcCode").innerHTML;
	Strings = Strings.replace(/;/g,";\n\t");

	if (typeof(document.getElementById("SrcCode").innerText)!= "undefined") {
		document.getElementById("SrcCode").innerText=Strings;
	} else {
		document.getElementById("SrcCode").innerHTML=Strings;
	}
}

function SheetChange()
{
	var select1 = document.forms.frm1.SheetNum;
	var select2 = document.forms.frm1.SelectorName;

	SyNum = select1.options[select1.selectedIndex].value;

	rules = window.opener.document.styleSheets[SyNum].cssRules  || window.opener.document.styleSheets[SyNum].rules;
	select2.options.length=0;
	select2.options[0] = new Option("","none");
	for(i=0; i < rules.length ;i++) {
		select2.options[i+1] = new Option(rules[i].selectorText,i);
	}
}

function ColorTable()
{
	document.write("		<TBODY>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#000000 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#000033 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#000066 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#000099 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#0000cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#0000ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#003300 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#003333 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#003366 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#003399 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#0033cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#0033ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#006600 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#006633 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#006666 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#006699 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#0066cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#0066ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#009900 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#009933 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#009966 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#009999 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#0099cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#0099ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#00cc00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#00cc33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#00cc66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#00cc99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#00cccc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#00ccff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#00ff00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#00ff33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#00ff66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#00ff99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#00ffcc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#00ffff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#330000 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#330033 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#330066 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#330099 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#3300cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#3300ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#333300 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#333333 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#333366 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#333399 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#3333cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#3333ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#336600 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#336633 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#336666 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#336699 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#3366cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#3366ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#339900 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#339933 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#339966 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#339999 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#3399cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#3399ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#33cc00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#33cc33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#33cc66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#33cc99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#33cccc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#33ccff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#33ff00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#33ff33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#33ff66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#33ff99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#33ffcc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#33ffff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#660000 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#660033 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#660066 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#660099 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#6600cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#6600ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#663300 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#663333 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#663366 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#663399 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#6633cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#6633ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#666600 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#666633 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#666666 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#666699 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#6666cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#6666ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#669900 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#669933 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#669966 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#669999 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#6699cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#6699ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#66cc00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#66cc33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#66cc66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#66cc99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#66cccc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#66ccff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#66ff00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#66ff33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#66ff66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#66ff99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#66ffcc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#66ffff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#990000 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#990033 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#990066 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#990099 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#9900cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#9900ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#993300 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#993333 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#993366 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#993399 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#9933cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#9933ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#996600 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#996633 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#996666 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#996699 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#9966cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#9966ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#999900 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#999933 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#999966 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#999999 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#9999cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#9999ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#99cc00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#99cc33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#99cc66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#99cc99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#99cccc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#99ccff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#99ff00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#99ff33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#99ff66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#99ff99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#99ffcc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#99ffff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#cc0000 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc0033 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc0066 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc0099 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc00cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc00ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#cc3300 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc3333 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc3366 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc3399 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc33cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc33ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#cc6600 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc6633 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc6666 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc6699 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc66cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc66ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#cc9900 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc9933 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc9966 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc9999 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc99cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cc99ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#cccc00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cccc33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cccc66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cccc99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#cccccc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ccccff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#ccff00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ccff33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ccff66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ccff99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ccffcc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ccffff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#ff0000 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff0033 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff0066 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff0099 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff00cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff00ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#ff3300 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff3333 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff3366 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff3399 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff33cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff33ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#ff6600 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff6633 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff6666 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff6699 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff66cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff66ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#ff9900 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff9933 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff9966 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff9999 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff99cc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ff99ff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#ffcc00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ffcc33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ffcc66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ffcc99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ffcccc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ffccff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		<TR>\n");
	document.write("			<TD width=40 bgColor=#ffff00 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ffff33 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ffff66 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ffff99 height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ffffcc height=10></TD>\n");
	document.write("			<TD width=40 bgColor=#ffffff height=10></TD>\n");
	document.write("		</TR>\n");
	document.write("		</TBODY>\n");
}
