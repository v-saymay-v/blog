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
oHelps["display"] = "��ʂɂǂ̂悤�ɕ\������邩�� block�i�u���b�N�v�f�j�Ainline�i�C�����C���v�f�j�Alist-item�i���X�g�v�f�j�Anone�i�\�����Ȃ��j �̂����ꂩ�Ŏw�肵�܂��B";
oHelps["position"] = "�|�W�V���j���O�̕��@���Astatic�i�K��l�F�{���̈ʒu�j�Arelative�i�{���̈ʒu����̑��Έʒu�w��j�Aabsolute�i��Έʒu�w��j�Afixed�i��Έʒu�ɌŒ�j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��Bfixed �͉�ʂ̃X�N���[���ɑ΂��ČŒ肳��܂��B";
oHelps["top"] = "��[����̋����� 10px �̂悤�ȒP�ʕt���̐��l�� 50% �̂悤�ȃp�[�Z���g�w��A�܂��� auto�i�K��l�F�����j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂�";
oHelps["left"] = "���[����̋����� 10px �̂悤�ȒP�ʕt���̐��l�� 50% �̂悤�ȃp�[�Z���g�w��A�܂��� auto�i�K��l�F�����j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂�";
oHelps["right"] = "�E�[����̋����� 10px �̂悤�ȒP�ʕt���̐��l�� 50% �̂悤�ȃp�[�Z���g�w��A�܂��� auto�i�K��l�F�����j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂�";
oHelps["bottom"] = "���[����̋����� 10px �̂悤�ȒP�ʕt���̐��l�� 50% �̂悤�ȃp�[�Z���g�w��A�܂��� auto�i�K��l�F�����j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂�";
oHelps["float"] = "�摜�Ȃǂ̕\���ʒu�� left�i���[�j�Aright�i�E�[�j�Anone�i�K��l�F�w�肵�Ȃ��j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["clear"] = "float: �ɂ��e�L�X�g�̉�荞�݂� none�i�K��l�F�������Ȃ��j�Aleft�i�����̉�肱�݉����j�Aright�i�E���̉�肱�݂������j�Aboth�i�����̉�肱�݂������j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["z-index"] = "�v�f���m���d�Ȃ��ĕ\�������ꍇ�́A�d�Ȃ�̏������w�肵�܂��Bz-index �Ɏw�肵�����l���傫�Ȃ��̂قǑO�ʂɕ\������܂��B";
oHelps["direction"] = "���͂̕����i���{���p��͍�����E�����A�A���r�A��͉E���獶�j���w�肵�܂��Bdirection �ɂ� ltr�i�K��l�F������E�j�Artl�i�E���獶�j�Ainherit�i�p���j�̂����ꂩ���w�肵�܂��B";
oHelps["unicode-bidi"] = "���͂̕����i���{���p��͍�����E�����A�A���r�A��͉E���獶�j���w�肵�܂��Bbidirection �ɂ� normal�i�K��l�F�ʏ�j�Abidi-override�idirection ��L���ɂ���j�Aembed�i�����ɂ���j�Ainherit�i�p���j�̂����ꂩ���w�肵�܂��B";
oHelps["width"] = "������ 100px �̂悤�ȒP�ʕt�����l�A50% �̂悤�ȃp�[�Z���g�`���Ainherit�i�p���j�Aauto�i�K��l�F�����j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["height"] = "������ 100px �̂悤�ȒP�ʕt�����l�A50% �̂悤�ȃp�[�Z���g�`���Ainherit�i�p���j�Aauto�i�K��l�F�����j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["vertical-align"] = "�c�����̔z�u���A�e�v�f�Ƃ̑��Έʒu�ŁAbaseline�i�x�[�X���C�����킹�j�Amiddle�i�������킹�j�Asub�i���t�������̈ʒu�j�Asuper�i��t�������̈ʒu�j�Atext-top�i�e�L�X�g�̏�����킹�j�Atext-bottom�i�e�L�X�g�̉������킹�j�Atop�i��[���킹�j�Abottom�i���[���킹�j�Ainherit�i�p���j�A70% �̂悤�ȃp�[�Z���g�`���A0.5em �̂悤�ȒP�ʕt���̐��l�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["overflow"] = "�̈���͂ݏo�����v�f�̈������Avisible�i�K��l�F�\������j�Ahidden�i�B���j�Ascroll�i�X�N���[���o�[�ŕ\������j�Aauto�i�����j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["overflow-x"] = "X�����̗̈���͂ݏo�����v�f�̈������Avisible�i�K��l�F�\������j�Ahidden�i�B���j�Ascroll�i�X�N���[���o�[�ŕ\������j�Aauto�i�����j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["overflow-y"] = "Y�����̗̈���͂ݏo�����v�f�̈������Avisible�i�K��l�F�\������j�Ahidden�i�B���j�Ascroll�i�X�N���[���o�[�ŕ\������j�Aauto�i�����j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["clip"] = "�\������͈͂� auto�i�K��l�F�����j�Ainherit�i�p���j�Arect(��[,�E�[,���[,���[) �`���̂����ꂩ�Ŏw�肵�܂��B";
oHelps["visibility"] = "�\�����邵�Ȃ����Avisible�i�K��l�F�\������j�Ahidden�i�\�����Ȃ��j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["cursor"] = "���̗v�f�Ƀ}�E�X���悹���Ƃ��̃}�E�X�J�[�\���̌`��� auto�i�K��l�F�����j�Acrosshair�i�\����j�Adefault�i�ʏ�̂��́j�Apointer�i�|�C���^�j�Amove�i�ړ��p�j�Ae-resize�i�E���T�C�Y�j�Ane-resize�i�E�ナ�T�C�Y�j�Anw-resize�i���ナ�T�C�Y�j�An-resize�i�ナ�T�C�Y�j�Ase-resize�i�E�����T�C�Y�j�Asw-resize�i�������T�C�Y�j�As-resize�i�����T�C�Y�j�Aw-resize�i�����T�C�Y�j�Atext�i�e�L�X�g�I���j�Await�i�҂��j�Ahelp�i�w���v�j�Ainherit �i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["ime-mode"] = "�e�L�X�g�t�H�[���ȂǂɃt�H�[�J�X���ړ��������̓��{��ϊ��iIME�j�̓�����Aauto�i�K��l�F�����j�Aactive�i�t�H�[�J�X�ړ����ɃI���j�Ainactive�i�t�H�[�J�X�ړ����ɃI�t�j�Adisabled�iIME�g�p���֎~�j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["behavior"] = "�X�^�C���V�[�g�ŃX�^�C�����ꊇ�w�肷��̂ɉ����āA�r�w�C�r�A�@�\�œ���i�U�镑���j���ꊇ�w�肷�邱�Ƃ��ł��܂��Bbehavior �ɂ́A�X�N���v�g�t�@�C���� URL ��A<object> �^�O�Ŏw�肵��ID�AIE5.0 �̃f�t�H���g�r�w�C�r�A���Ȃǂ��w�肵�܂��B";
oHelps["filter"] = "�t�B���^�Ƃ́A�X�^�C���V�[�g�̊g���Ƃ��� Internet Explorer 4.0 �ŃT�|�[�g���ꂽ�@�\�ł��B������摜�ɑ΂��ĉe�t����ڂ����Ȃǂ̃G�t�F�N�g�������邱�Ƃ��ł��܂��B";
oHelps["scrollbar-base-color"] = "�X�N���[���o�[�̂��ׂĂ̗v�f�̐F���܂Ƃ߂Đݒ肵�܂��B";
oHelps["scrollbar-track-color"] = "�X�N���[���o�[�̐F���w�肵�܂��B";
oHelps["scrollbar-face-color"] = "�X�N���[���o�[�̐F���w�肵�܂��B";
oHelps["scrollbar-shadow-color"] = "�X�N���[���o�[�̐F���w�肵�܂��B";
oHelps["scrollbar-highlight-color"] = "�X�N���[���o�[�̐F���w�肵�܂��B";
oHelps["scrollbar-3dlight-color"] = "�X�N���[���o�[�̐F���w�肵�܂��B";
oHelps["scrollbar-arrow-color"] = "�X�N���[���o�[�̐F���w�肵�܂��B";
oHelps["list-style"] = "<li> �^�O�ŕ\�������}�[�J�[�̌`�A�}�[�J�[�̈ʒu�A�}�[�J�[�̃C���[�W����x�Ɏw�肵�܂��B";
oHelps["list-style-type"] = "<li> �^�O�ŕ\�������}�[�J�[�̌`�� disc�i�K��l�F���ہj�Acircle�i���ہj�Asquare�i�l�p�j�Adecimal�i1, 2, 3...�j�Alower-roman�ii, ii, iii...�j�Aupper-roman�iI, II, III...�j�Alower-alpha�ia, b, c...�j�Aupper-alpha�iA, B, C...�j�Adecimal-leading-zero�i01, 02, 03...�j�Alower-greek�i���A���A��...�j�Aupper-greek�i���A���A��...�j�Alower-latin�i���������e�������j�Aupper-latin�i�啶�����e�������j�Ahebrew�i�w�u���C�����j�Aarmenian�i�A�����j�A�����j�Ageorgian�i�O���W�A�����j�Acjk-ideographic�i��A��A�O...�j�Ahiragana�i���A���A��...�j�Akatakana�i�A�A�C�A�E...�j�Ahiragana-iroha�i���A��A��...�j�Akatakana-iroha�i�C�A���A�n...�j�Anone�i�Ȃ��j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["list-style-position"] = "�}�[�J�[�̈ʒu�� outside�i�K��l�F�O���j�Ainside�i�����j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["list-style-image"] = "<li> �^�O�ŕ\�������}�[�J�[�̃C���[�W�� URL�Anone�i�����j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["page-break-before"] = "������ɗv�f�̒��O�ŉ��y�[�W���܂��B";
oHelps["page-break-after"] = "������ɗv�f�̒���ŉ��y�[�W���܂��B";
oHelps["background"] = "color, image, repeat, attachment, position ����x�Ɏw�肵�܂��B";
oHelps["background-color"] = "�w�i�F���w�肵�܂��Bcolor �ɂ͐F�̖��O�Atransparent�i�K��l�F�����F�j�Ainherit�i�p���j�̂����ꂩ���w�肵�܂��B";
oHelps["background-image"] = "�w�i�摜�� URL�A�܂��� none�i�K��l�F�摜�Ȃ��j�Ainherit�i�p���j�̂����ꂩ���w�肵�܂��B";
oHelps["background-attachment"] = "�E�B���h�E�̃X�N���[���𓮂��������̔w�i�̓���� scroll�i�K��l�F�ꏏ�ɃX�N���[������j�Afixed�i�X�N���[�����Ȃ��j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["background-repeat"] = "�w�i�摜�̕��ׂ����� repeat�i�K��l�F�~���l�߂�j�Arepeat-x�i�������̂ݕ��ׂ�j�Arepeat-y�i�c�����̂ݕ��ׂ�j�Ano-repeat�i�ЂƂ����\������j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["background-position"] = "�w�i�̉������̈ʒu�� left�i���[�j�Acenter�i�����j�Aright�i�E�[�j�܂��� 50% �̂悤�Ȋ����Ŏw�肵�A�c�����̈ʒu�� top�i��[�j�Acenter�i�����j�Abottom�i���[�j�܂��� 50% �̂悤�Ȋ����Ŏw�肵�܂��B";
oHelps["color"] = "�F���w�肵�܂��B";
oHelps["font-style"] = "normal�i�K��l�F�ʏ�j, italic�i�C�^���b�N�j, oblique�i�΂߁j�̂����ꂩ���w�肵�܂��B";
oHelps["font-variant"] = "normal�i�K��l�F�ʏ�j, small-caps�i�啶���j�̂����ꂩ���w�肵�܂��B";
oHelps["font-weight"] = "�t�H���g�̑����� normal�i�K��l�j, bold, bolder, lighter, 100, 200, 300, 400, 500, 600, 700, 800, 900 �̂����ꂩ�Ŏw�肵�܂��B";
oHelps["font-size"] = "�t�H���g�̑傫�����w�肵�܂��B��Ύw��Ƃ��� xx-small, x-small, small, medium�i�K��l�j, large, x-large, xx-large�A���Ύw��Ƃ��� larger, smaller�A��ΒP�ʎw��Ƃ��� 10in, 10cm, 10mm, 10pt, 10pc, ���ΒP�ʎw��Ƃ��� 10px, 10ex, 10em �Ȃǂ��A�����w��Ƃ��� 120% �Ȃǂ��w�肵�܂��B";
oHelps["line-height"] = "�e�L�X�g�̍����� 1.5em, 150% �ȂǂŎw�肵�܂��B";
oHelps["font-family"] = "�t�H���g���w�肵�܂��B�J���}�i,�j�ŕ����L�q����ƁA�w�肵���t�H���g��������Ύ��̃t�H���g���̗p����܂��B";
oHelps["text-indent"] = "�e�L�X�g�̍ŏ��� 1�s�̃C���f���g�i�������j�� 1em �̂悤�ȒP�ʕt���̐��l�A10% �̂悤�ȃp�[�Z���g�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["text-align"] = "�e�L�X�g�̔z�u�� left�i�K��l�F���񂹁j�Aright�i�E�񂹁j�Acenter�i�Z���^�����O�j�Ajustify�i���[�����j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["text-justify"] = "�P��Ԃ╶���Ԃ̃X�y�[�X�𒲐����āA���͂̕\����̉E�[���킹���s���܂��Bjustify �ɂ́Aauto�i�K��l�j�Anewspaper�i�p�������j�Ainter-word�i�P��Ԃ݂̂ɂ�钲���j�Adistribute-all-lines�i�Ō�̍s�������j�Ainter-ideograph�i�P��Ԃƕ����Ԃɂ�钲���j�Ainter-cluster�i�A�W�A��������j�Adistribute�i�^�C��������j�Akashida�i�A���r�A������j�Ȃǂ��w�肵�܂��B";
oHelps["text-decoration"] = "�e�L�X�g�̑����� none�i�����Ȃ��j�Aunderline�i�����j�Aoverline�i����j�Aline-through�i�ł��������j�Ablink�i�_�Łj�Ainherit�i�p���j�Ŏw�肵�܂��B�����w����\�ł��B";
oHelps["text-underline-position"] = "�����̏ꏊ���Abelow�i�K��l�F�����j�Aabove�i�㑤�j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["letter-spacing"] = "�e�����Ԃ̃X�y�[�X�� normal�i�K��l�j�A2px �Ȃǂ̒����Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["word-spacing"] = "�e���[�h�Ԃ̃X�y�[�X�� normal�i�K��l�j�A10px �Ȃǂ̒����Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["text-transform"] = "�啶���A�������ϊ��ɂ��āAcapitalize�i�e�P��̍ŏ��̕�����啶���ɂ���j�Auppercase�i���ׂđ啶���ɂ���j�Alowercase�i���ׂď������ɂ���j�Anone�i�K��l�F�������Ȃ��j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["white-space"] = "�v�f�̒��̋󔒂��ǂ̂悤�Ɉ����邩�� normal�i�K��l�F�ʏ�j�Apre�i�������s���Ȃ��A�󔒂��̂܂܁j�Anowrap�i�������s���Ȃ��A�󔒂͂߂�j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["line-break"] = "�s���֑̋��������Anormal�i�K��l�F�ʏ�ʂ�u�B�v��u�A�v���֑����������j�Astrict�i�u���v��u��v�Ȃǂ��֑����������j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["word-break"] = "�s���̒P��֑��������Anormal�i�K��l�F�p���̂ݒP��̐؂�ڂŉ��s�j�Abreak-all�i�p���A�a�����ɒP��̓r���ł����s�j�Akeep-all�i�p���A�a�����ɒP��̐؂�ڂŉ��s�j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["ruby-align"] = "<ruby> �v�f�ɑ΂��ă��r�̔z�u���Aauto�i�K��l�F�����j�Aleft�i����j�Acenter�i�����j�Aright�i�E��j�Adistribute-letter�i���[�����j�Adistribute-space�i�ϊ��j�Aline-edge�i�s���ł̏����ύX�j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["ruby-overhang"] = "���r�������ꍇ�̂͂ݏo����� auto�i�K��l�F�͂ݏo���j�Awhitespace�i�X�y�[�X�������܂ł͂͂ݏo���j�Anone�i�͂ݏo���Ȃ��j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["ruby-position"] = "<ruby> �v�f�ɑ΂��ă��r�̔z�u���Aabove�i�K��l�F�㕔�j�Ainline�i���j�̂����ꂩ�Ŏw�肵�܂��Binline ���w�肵���ꍇ�́A<ruby> ���Ή��̃u���E�U�̂悤�ɕ\������܂��B";
oHelps["layout-grid"] = "�s�Ԃ╶���Ԃ̃X�y�[�X�𒲐����܂��B";
oHelps["layout-grid-line"] = "�s�Ԃ̃X�y�[�X�� none�i�K��l�F�w�薳���j�Aauto�i�����j�A3px �� 1em �Ȃǂ̒����⊄���i%�j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["layout-grid-char"] = "�����Ԃ̃X�y�[�X�� none�i�K��l�F�w�薳���j�Aauto�i�����j�A3px �� 1em �Ȃǂ̒����⊄���i%�j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["layout-grid-mode"] = "mode �ɂ� both�i�K��l�Fline �� char �̗�����K�p����j�Anone�i�ǂ�����K�p���Ȃ��j�Aline�iline �݂̂�K�p����j�Achar�ichar �݂̂�K�p����j�̂����ꂩ���w�肵�܂��B";
oHelps["layout-grid-type"] = "type �ɂ́Aloose�i�K��l�F���{���؍���ɓK�������@�j�Astrict�i������ɂ��K�������@�j�Afixed�i�Œ�j�̂����ꂩ���w�肵�܂��B";
oHelps["text-autospace"] = "�����̂悤�ȕ\�ӕ����Ɣ�\�ӕ����̊Ԃɂق�̏����������Ԃ����邩�ǂ������Anone�i�K��l�F���Ԃ����Ȃ��j�Aideograph-alpha�i�\�ӕ����Ɣ�\�ӕ����̊Ԃɓ����j�Aideograph-numeric�i�\�ӕ����Ɛ����̊Ԃɓ����j�Aideograph-parenthesis�i�\�ӕ����Ɗ��ʂ̊Ԃɓ����j�Aideograph-space�i�\�ӕ����̈ʒu�ɓK������悤�ɓ����j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["text-kashida-space"] = "�A���r�A��Ȃǂ̃e�L�X�g�z�u���őP������ۂɗp���܂��Bkashida �ɂ� 50% �̂悤�ȃp�[�Z���g���Ainherit�i�p���j�̂����ꂩ���w�肵�܂��B";
oHelps["writing-mode"] = "�c�������������܂��Bmode �ɂ� lr-tb�i�������Fleft to right, top to bottom�j�Atb-rl�i�c�����Ftop to bottom, right to left�j�̂����ꂩ���w�肵�܂��B";
oHelps["caption-side"] = "�e�[�u���̃L���v�V�����̈ʒu�� top�i��j�Abottom�i���j�Aright�i�E�j�Aleft�i���j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��BNetscape 6 �ł� top �� bottom �̂ݗL���ł��B";
oHelps["table-layout"] = "�e�[�u���̃��C�A�E�g���@�� auto�i�K��l�F�����j�Afixed�i�Œ�j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��Bfixed ���w�肷��ƁA�e�[�u���̉������e�[�u���̓��e�Ɋւ�炸�Œ肷�邱�Ƃ��ł��܂��B";
oHelps["border-collapse"] = "�e�[�u���̊e�Z���̘g���� collapse�i�ׂ̃Z���̘g���Əd�˂ĕ\���j�Aseparate�i�ׂ̃Z���Ƃ����������ĕ\���j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["border"] = "�㉺���E���ׂẴ{�[�_�[�i�g���j�� width, style, color ����x�Ɏw�肵�܂��B";
oHelps["border-color"] = "�㉺���E���ׂẴ{�[�_�[�i�g���j�̐F�� red �̂悤�ȐF�̖��O���A#ff0000 �̂悤�� RGB�`���ȂǂŎw�肵�܂��B";
oHelps["border-width"] = "�㉺���E���ׂẴ{�[�_�[�i�g���j�̑������Athin�i�א��j�Amedium�i�K��l�F�������j�Athick�i�����j�̂����ꂩ�A�܂��� 1px�i���w��j �Ȃǂ̂悤�ȒP�ʕt���̐��l�Ŏw�肵�܂��B";
oHelps["border-style"] = "�㉺���E���ׂẴ{�[�_�[�i�g���j�̃X�^�C�����Anone�i�������j�Adotted�i�_���j�Adashed�i�e���_���j�Asolid�i�����j�Adouble�i��d���j�Agroove�i�J���j�Aridge�i�R���j�Ainset�i�����j�Aoutset�i�O���j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["border-top"] = "��̃{�[�_�[�i�g���j�� width, style, color ����x�Ɏw�肵�܂��B";
oHelps["border-top-color"] = "��̃{�[�_�[�i�g���j�̐F�� red �̂悤�ȐF�̖��O���A#ff0000 �̂悤�� RGB�`���ȂǂŎw�肵�܂��B";
oHelps["border-top-width"] = "��̃{�[�_�[�i�g���j�̑������Athin�i�א��j�Amedium�i�K��l�F�������j�Athick�i�����j�̂����ꂩ�A�܂��� 1px�i���w��j �Ȃǂ̂悤�ȒP�ʕt���̐��l�Ŏw�肵�܂��B";
oHelps["border-top-style"] = "��̃{�[�_�[�i�g���j�̃X�^�C�����Anone�i�������j�Adotted�i�_���j�Adashed�i�e���_���j�Asolid�i�����j�Adouble�i��d���j�Agroove�i�J���j�Aridge�i�R���j�Ainset�i�����j�Aoutset�i�O���j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["border-left"] = "���̃{�[�_�[�i�g���j�� width, style, color ����x�Ɏw�肵�܂��B";
oHelps["border-left-color"] = "���̃{�[�_�[�i�g���j�̐F�� red �̂悤�ȐF�̖��O���A#ff0000 �̂悤�� RGB�`���ȂǂŎw�肵�܂��B";
oHelps["border-left-width"] = "���̃{�[�_�[�i�g���j�̑������Athin�i�א��j�Amedium�i�K��l�F�������j�Athick�i�����j�̂����ꂩ�A�܂��� 1px�i���w��j �Ȃǂ̂悤�ȒP�ʕt���̐��l�Ŏw�肵�܂��B";
oHelps["border-left-style"] = "���̃{�[�_�[�i�g���j�̃X�^�C�����Anone�i�������j�Adotted�i�_���j�Adashed�i�e���_���j�Asolid�i�����j�Adouble�i��d���j�Agroove�i�J���j�Aridge�i�R���j�Ainset�i�����j�Aoutset�i�O���j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["border-right"] = "�E�̃{�[�_�[�i�g���j�� width, style, color ����x�Ɏw�肵�܂��B";
oHelps["border-right-color"] = "�E�̃{�[�_�[�i�g���j�̐F�� red �̂悤�ȐF�̖��O���A#ff0000 �̂悤�� RGB�`���ȂǂŎw�肵�܂��B";
oHelps["border-right-width"] = "�E�̃{�[�_�[�i�g���j�̑������Athin�i�א��j�Amedium�i�K��l�F�������j�Athick�i�����j�̂����ꂩ�A�܂��� 1px�i���w��j �Ȃǂ̂悤�ȒP�ʕt���̐��l�Ŏw�肵�܂��B";
oHelps["border-right-style"] = "�E�̃{�[�_�[�i�g���j�̃X�^�C�����Anone�i�������j�Adotted�i�_���j�Adashed�i�e���_���j�Asolid�i�����j�Adouble�i��d���j�Agroove�i�J���j�Aridge�i�R���j�Ainset�i�����j�Aoutset�i�O���j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["border-bottom"] = "���̃{�[�_�[�i�g���j�� width, style, color ����x�Ɏw�肵�܂��B";
oHelps["border-bottom-color"] = "���̃{�[�_�[�i�g���j�̐F�� red �̂悤�ȐF�̖��O���A#ff0000 �̂悤�� RGB�`���ȂǂŎw�肵�܂��B";
oHelps["border-bottom-width"] = "���̃{�[�_�[�i�g���j�̑������Athin�i�א��j�Amedium�i�K��l�F�������j�Athick�i�����j�̂����ꂩ�A�܂��� 1px�i���w��j �Ȃǂ̂悤�ȒP�ʕt���̐��l�Ŏw�肵�܂��B";
oHelps["border-bottom-style"] = "���̃{�[�_�[�i�g���j�̃X�^�C�����Anone�i�������j�Adotted�i�_���j�Adashed�i�e���_���j�Asolid�i�����j�Adouble�i��d���j�Agroove�i�J���j�Aridge�i�R���j�Ainset�i�����j�Aoutset�i�O���j�Ainherit�i�p���j�̂����ꂩ�Ŏw�肵�܂��B";
oHelps["margin"] = "�㉺���E�̃}�[�W���i�]���j�� 20px, 3em�A10% �Ȃǂ̂悤�Ɏw�肵�܂��B";
oHelps["margin-top"] = "��̃}�[�W���i�]���j�� 20px, 3em�A10% �Ȃǂ̂悤�Ɏw�肵�܂��B";
oHelps["margin-left"] = "���̃}�[�W���i�]���j�� 20px, 3em�A10% �Ȃǂ̂悤�Ɏw�肵�܂��B";
oHelps["margin-right"] = "�E�̃}�[�W���i�]���j�� 20px, 3em�A10% �Ȃǂ̂悤�Ɏw�肵�܂��B";
oHelps["margin-bottom"] = "���̃}�[�W���i�]���j�� 20px, 3em�A10% �Ȃǂ̂悤�Ɏw�肵�܂��B";
oHelps["padding"] = "�㉺���E�̃p�f�B���O�i�]���j�� 2.0em, 2em, 120% �Ȃǂ̂悤�Ɏw�肵�܂��B";
oHelps["padding-top"] = "��̃p�f�B���O�i�]���j�� 2.0em, 2em, 120% �Ȃǂ̂悤�Ɏw�肵�܂��B";
oHelps["padding-left"] = "���̃p�f�B���O�i�]���j�� 2.0em, 2em, 120% �Ȃǂ̂悤�Ɏw�肵�܂��B";
oHelps["padding-right"] = "�E�̃p�f�B���O�i�]���j�� 2.0em, 2em, 120% �Ȃǂ̂悤�Ɏw�肵�܂��B";
oHelps["padding-bottom"] = "���̃p�f�B���O�i�]���j�� 2.0em, 2em, 120% �Ȃǂ̂悤�Ɏw�肵�܂��B";

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
	AddTab("General", "�S��");
	AddTab("List", "���X�g");
	AddTab("Print", "���");
	AddTab("Background", "�w�i");
	AddTab("Font", "�t�H���g");
	AddTab("Text", "�e�L�X�g");
	AddTab("Table", "�e�[�u��");
	AddTab("Border", "�{�[�_�[");
	AddTab("Margin", "�}�[�W��");
	AddTab("Padding", "�p�f�B���O");
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
		// �S��
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

		// �}�[�W��
		document.getElementsByName("margin")[0].value=CurStyle.margin;
		document.getElementsByName("marginBottom")[0].value=CurStyle.marginBottom;
		document.getElementsByName("marginLeft")[0].value=CurStyle.marginLeft;
		document.getElementsByName("marginRight")[0].value=CurStyle.marginRight;
		document.getElementsByName("marginTop")[0].value=CurStyle.marginTop;

		// �p�f�B���O
		document.getElementsByName("padding")[0].value=CurStyle.padding;
		document.getElementsByName("paddingBottom")[0].value=CurStyle.paddingBottom;
		document.getElementsByName("paddingLeft")[0].value=CurStyle.paddingLeft;
		document.getElementsByName("paddingRight")[0].value=CurStyle.paddingRight;
		document.getElementsByName("paddingTop")[0].value=CurStyle.paddingTop;

		//  �{�[�_�[
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

		//  ���X�g
		document.getElementsByName("listStyle")[0].value = CurStyle.listStyle;
		document.getElementsByName("listStyleImage")[0].value = CurStyle.listStyleImage;
		SelectCombo(document.getElementsByName("listStyleType")[0], CurStyle.listStyleType);
		SelectCombo(document.getElementsByName("listStylePosition")[0], CurStyle.listStyleType);

		//  ���
		SelectCombo(document.getElementsByName("pageBreakBefore")[0], CurStyle.pageBreakBefore);
		SelectCombo(document.getElementsByName("pageBreakAfter")[0], CurStyle.pageBreakAfter);

		//  �w�i
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

		//  �t�H���g
//		document.getElementsByName("fontAll")[0].value=CurStyle.font;
		document.getElementsByName("color")[0].value=CurStyle.color;
		SelectCombo(document.getElementsByName("fontStyle")[0], CurStyle.fontStyle);
		SelectCombo(document.getElementsByName("fontVariant")[0], CurStyle.fontVariant);
		SelectCombo(document.getElementsByName("fontWeight")[0], CurStyle.fontWeight);
		document.getElementsByName("fontSize")[0].value=CurStyle.fontSize;
		document.getElementsByName("lineHeight")[0].value=CurStyle.lineHeight;
		document.getElementsByName("fontFamily")[0].value=CurStyle.fontFamily;

		//  �e�L�X�g
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

		//  �e�[�u��
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

		// �S��
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

		// �}�[�W��
		if (typeof(CurStyle.margin) != "undefined") CurStyle.margin = document.getElementsByName("margin")[0].value;
		if (typeof(CurStyle.marginBottom) != "undefined") CurStyle.marginBottom = document.getElementsByName("marginBottom")[0].value;
		if (typeof(CurStyle.marginLeft) != "undefined") CurStyle.marginLeft = document.getElementsByName("marginLeft")[0].value;
		if (typeof(CurStyle.marginRight) != "undefined") CurStyle.marginRight = document.getElementsByName("marginRight")[0].value;
		if (typeof(CurStyle.marginTop) != "undefined") CurStyle.marginTop = document.getElementsByName("marginTop")[0].value;

		// �p�f�B���O
		if (typeof(CurStyle.padding) != "undefined") CurStyle.padding = document.getElementsByName("padding")[0].value;
		if (typeof(CurStyle.paddingBottom) != "undefined") CurStyle.paddingBottom = document.getElementsByName("paddingBottom")[0].value;
		if (typeof(CurStyle.paddingLeft) != "undefined") CurStyle.paddingLeft = document.getElementsByName("paddingLeft")[0].value;
		if (typeof(CurStyle.paddingRight) != "undefined") CurStyle.paddingRight = document.getElementsByName("paddingRight")[0].value;
		if (typeof(CurStyle.paddingTop) != "undefined") CurStyle.paddingTop = document.getElementsByName("paddingTop")[0].value;

		// �{�[�_�[
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

		//  ���X�g
		if (typeof(CurStyle.listStyle) != "undefined") CurStyle.listStyle = document.getElementsByName("listStyle")[0].value;
		if (typeof(CurStyle.listStyleImage) != "undefined") CurStyle.listStyleImage = document.getElementsByName("listStyleImage")[0].value;
		if (typeof(CurStyle.listStyleType) != "undefined") CurStyle.listStyleType = document.getElementsByName("listStyleType")[0].value;
		if (typeof(CurStyle.listStyleType) != "undefined") CurStyle.listStyleType = document.getElementsByName("listStylePosition")[0].value;

		//  ���
		if (typeof(CurStyle.pageBreakBefore) != "undefined") CurStyle.pageBreakBefore = document.getElementsByName("pageBreakBefore")[0].value;
		if (typeof(CurStyle.pageBreakAfter) != "undefined") CurStyle.pageBreakAfter = document.getElementsByName("pageBreakAfter")[0].value;

		// �w�i
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

		// �t�H���g
//		if (typeof(CurStyle.font) != "undefined") CurStyle.font = document.getElementsByName("fontAll")[0].value;
		if (typeof(CurStyle.color) != "undefined") CurStyle.color = document.getElementsByName("color")[0].value;
		if (typeof(CurStyle.fontStyle) != "undefined") CurStyle.fontStyle = document.getElementsByName("fontStyle")[0].value;
		if (typeof(CurStyle.fontVariant) != "undefined") CurStyle.fontVariant = document.getElementsByName("fontVariant")[0].value;
		if (typeof(CurStyle.fontWeight) != "undefined") CurStyle.fontWeight = document.getElementsByName("fontWeight")[0].value;
		if (typeof(CurStyle.fontSize) != "undefined") CurStyle.fontSize = document.getElementsByName("fontSize")[0].value;
		if (typeof(CurStyle.fontHeight) != "undefined") CurStyle.fontHeight = document.getElementsByName("fontHeight")[0].value;
		if (typeof(CurStyle.fontFamily) != "undefined") CurStyle.fontFamily = document.getElementsByName("fontFamily")[0].value;

		// �e�L�X�g
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

		//  �e�[�u��
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
