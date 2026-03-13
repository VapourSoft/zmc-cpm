
SECTION code_user

PUBLIC _z3vinit, _gxymsg, _stndout, _stndend, _gotoxy, _cls, _clreos, _ereol
PUBLIC _vprint, _vpstr, _dellin, _inslin, _curoff, _curon

EXTERN Z3VINIT, GXYMSG, STNDOUT, STNDEND, GOTOXY, CLS, CLREOS, EREOL
EXTERN VPRINT, VPSTR, DELLIN, INSLIN, CUROFF, CURON

aret:
    ld   h,0
    ld   l,a
    ret

_z3vinit:   
    call Z3VINIT
    ret

_stndout:   
    call STNDOUT
    jr   aret

_stndend:
    call STNDEND
    jr   aret

_gotoxy:    
    call GOTOXY
    jr   aret

_gxymsg:
    push hl
    ld  hl,GXYMSG
    jp  (hl)

_vprint:    
    push hl
    ld  hl,VPRINT
    jp  (hl)

_vpstr: 
    call VPSTR
    jr  aret

_dellin:    
    call DELLIN
    jr  aret

_inslin:    
    call INSLIN
    jr  aret

_cls:
    call CLS
    jr  aret

_clreos:    
    call CLREOS
    jr  aret

_ereol: 
    call EREOL
    jr  aret

_curoff:    
    call CUROFF
    jr  aret

_curon: 
    call CURON
    jr  aret

