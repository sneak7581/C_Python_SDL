#include "../engine/core.h"
#include "../engine/tile.h"
#include "cmd_parser.h"

tDE_S_Core *g_pEngineCore;
SDL_Texture *g_pTileSet;
Sint16 g_worldMap_Layer_1[64];
Sint16 g_attrMap_Layer_1[64]; // 속성 맵
Uint16 g_nSelectTileIndex = 0;

SDL_Rect g_rtTilePalette = {440, 0, 160, 350}; // 팔레트 영역 전역으로 선언
SDL_Rect g_rtWroldMap = {0, 0, 256, 256};      // 월드맵그리는 영역 전역으로

SDL_bool g_bDrawGrid = SDL_TRUE; // 그리드모드 일 때는 속성편집 가능, 아닐때는 타일 찍을 수 있도록 기능구현 버튼 다운에 넣을예정
SDL_bool g_nCurrentAttr = 0;     // 현재 속성 0으로 설정

int main(int argc, char *argv[])
{
  //월드맵 초기화 기능 아래 memset 기능 사용해서 처리함
  // for (int i = 0; i < 64; i++)
  // {
  //   g_worldMap_Layer_1[i] = -1;
  // }
  memset(g_worldMap_Layer_1, -1, sizeof(Sint16) * 64); // 월드맵 초기화
  memset(g_attrMap_Layer_1, 0, sizeof(Sint16) * 64);   // 속성맵 초기화

  setup_cmd_parser(g_worldMap_Layer_1);
  g_pEngineCore = tDE_setup_1("maptool", 640, 480, 0);
  g_pTileSet = tDE_util_loadTexture(g_pEngineCore, "../../res/dungeon.png");

  SDL_bool bLoop = SDL_TRUE;
  while (bLoop)
  {
    SDL_Renderer *pRender = g_pEngineCore->m_pRender;
    SDL_SetRenderDrawColor(pRender, 0, 0, 0, 1);
    SDL_RenderClear(pRender);

    //select tile
    putTile(pRender, g_pTileSet, 15, 0, g_nSelectTileIndex, 16, 5, 2);

    //440,100
    {
      //팔래트 랜디링
      SDL_SetRenderDrawColor(pRender, 0xff, 0xff, 0xff, 0xff);
      // 팔레트 영역 선언한 부분 전역으로 올림
      // SDL_Rect _rtTemp;
      // _rtTemp.x = 440;
      // _rtTemp.y = 0;
      // _rtTemp.w = 160;
      // _rtTemp.h = 350;

      SDL_RenderDrawRect(pRender, &g_rtTilePalette);

      SDL_Rect _dstRt = {440, 100, 80 * 2, 80 * 2};
      SDL_RenderCopy(pRender, g_pTileSet, NULL, &_dstRt);
    }

    //render world map
    {
      SDL_SetRenderDrawColor(pRender, 0xff, 0xff, 0xff, 0xff);
      // 월드맵 영역 선언한 부분 전역으로 올림
      // SDL_Rect _rtTemp;
      // _rtTemp.x = 0;
      // _rtTemp.y = 0;
      // _rtTemp.w = 256;
      // _rtTemp.h = 256;

      SDL_RenderDrawRect(pRender, &g_rtWroldMap);

      // _index = -1 열린블럭
      for (int i = 0; i < 64; i++)
      {
        Sint16 _index = g_worldMap_Layer_1[i];
        if (_index != -1)
        {
          putTile(pRender, g_pTileSet, i % 8, i / 8, _index, 16, 5, 2);
        }
      }

      // 막힌 블럭 구현 attr
      for (int i = 0; i < 64; i++)
      {
        Sint16 _index = g_attrMap_Layer_1[i];
        if (_index == 1)
        {
          SDL_SetRenderDrawBlendMode(pRender,SDL_BLENDMODE_BLEND); // 반투명 모드 설정
          SDL_SetRenderDrawColor(pRender, 0xff, 0xff, 0xff, 0x80);
          SDL_Rect _rt = {(i % 8) * 32, (i / 8) * 32, 32, 32};
          SDL_RenderFillRect(pRender, &_rt);
          SDL_SetRenderDrawBlendMode(pRender, SDL_BLENDMODE_NONE); // 반투명 모드 끄기 (오버헤드 많이나서 꺼줘야함)
        }
      }

      // 문 속성 구현 attr 인덱스 2번
      for (int i = 0; i < 64; i++)
      {
        Sint16 _index = g_attrMap_Layer_1[i];
        if (_index == 2 )
        {
          SDL_SetRenderDrawBlendMode(pRender,SDL_BLENDMODE_BLEND); // 반투명 모드 설정
          SDL_SetRenderDrawColor(pRender, 0x00, 0xff, 0x00, 0x80);
          SDL_Rect _rt = {(i % 8) * 32, (i / 8) * 32, 32, 32};
          SDL_RenderFillRect(pRender, &_rt);
          SDL_SetRenderDrawBlendMode(pRender, SDL_BLENDMODE_NONE); // 반투명 모드 끄기 (오버헤드 많이나서 꺼줘야함)
        }
      }

      //그리드 그리기
      if (g_bDrawGrid)
      {
        for (int ix = 0; ix < 8; ix++)
        {
          SDL_SetRenderDrawColor(pRender, 0xff, 0xff, 0x00, 0xff);
          SDL_RenderDrawLine(pRender, ix * 32, 0, ix * 32, 256);
        }
        for (int iy = 0; iy < 8; iy++)
        {
          SDL_SetRenderDrawColor(pRender, 0xff, 0xff, 0x00, 0xff);
          SDL_RenderDrawLine(pRender, 0, iy * 32, 256, iy * 32);
        }
      }
    }

    SDL_RenderPresent(pRender);

    SDL_Event _event;
    while (SDL_PollEvent(&_event))
    {
      static char szBuf[64];
      static Uint16 nInputFSM = 0;

      switch (_event.type)
      {
      case SDL_MOUSEMOTION: // 마우스 모션
      {
        SDL_Point m_pt = {_event.motion.x, _event.motion.y};

        if (_event.button.button == 1) // 좌클릭
        {

          if (SDL_PointInRect(&m_pt, &g_rtWroldMap) && !g_bDrawGrid) // 그리드모드 안켜져있을때
          {
            putMap(m_pt.x / 32, m_pt.y / 32, g_nSelectTileIndex, g_worldMap_Layer_1, 8);
          }
        }
        else if (_event.button.button == 4 && !g_bDrawGrid) // 우클릭 + 드래그 그리드모드 안켜져있을때
          if (SDL_PointInRect(&m_pt, &g_rtWroldMap) == SDL_TRUE)
          {
            putMap(m_pt.x / 32, m_pt.y / 32, -1, g_worldMap_Layer_1, 8);
          }

        printf("%4d%4d\r", _event.motion.x, _event.motion.y);
      }
      break;

      case SDL_MOUSEBUTTONDOWN: // 마우스 클릭
      {
        // 현재 마우스 포인터 위치
        SDL_Point mousePt = {_event.motion.x, _event.motion.y};
        printf("%8d\r", _event.button.button);
        if (_event.button.button == 1) //마우스 좌클릭
        {

          //팔래트처리
          if (SDL_PointInRect(&mousePt, &g_rtTilePalette)) // 팔레트 영역구분처리 영역안에 있으면참
          {
            int _x = (_event.motion.x - 440) / 32;
            int _y = (_event.motion.y - 100) / 32;

            if ((_x >= 0 && _y >= 0) && (_x < 5 && _y < 5))
            {
              g_nSelectTileIndex = _y * 5 + _x;
            }
          }
          //월드멥처리
          else if (SDL_PointInRect(&mousePt, &g_rtWroldMap)) // 월드맵 영역구분처리
          {
            int _x = (_event.motion.x) / 32;
            int _y = (_event.motion.y) / 32;

            if (g_bDrawGrid) // 그리드모드일때 속성값 받아옴
            {
              // 현재속성을 마우스 클릭한 월드맵타일에 넣어줌
              printf("%3d,%3d,%3d,%3d\r", _x, _y,
                     g_attrMap_Layer_1[_x + _y * 8],
                     g_attrMap_Layer_1[_x + _y * 8]);
            }
            else // 아니면 타일찍기
            {
              putMap(_x, _y, g_nSelectTileIndex, g_worldMap_Layer_1, 8);
            }
          }
        }
        else if (_event.button.button == 3) //마우스 우클릭
          if (SDL_PointInRect(&mousePt, &g_rtWroldMap))
          {
            int _x = (_event.motion.x) / 32;
            int _y = (_event.motion.y) / 32;
            putMap(_x, _y, -1, g_worldMap_Layer_1, 8); // 지우개
          }
      }
      break;

      case SDL_KEYDOWN: // 키보드입력

        //printf("%d \n", _event.key.keysym.scancode);
        {
          switch (nInputFSM)
          {
          case 0: //대기상태
          {
            if (_event.key.keysym.sym == SDLK_RETURN)
            {
              printf("input cmd => \n");
              nInputFSM = 1; //입력 상태로 전이
            }
            else if (_event.key.keysym.sym == SDLK_g)
            {
              g_bDrawGrid = !g_bDrawGrid;
            }
          }
          break;
          case 1: //입력 상태
          {

            if (_event.key.keysym.sym == SDLK_RETURN)
            {
              nInputFSM = 0; //대기 상태로 전이
              // printf("\n%s\n",szBuf);
              //cmd parser
              {
                parseCmd(szBuf);
              }

              szBuf[0] = 0x00; //문자열 클리어
            }
            else if (_event.key.keysym.sym == SDLK_BACKSPACE)
            {
              int _len = strlen(szBuf);
              szBuf[_len - 1] = 0x00;
              printf("%s  \r", szBuf);
            }
          }
          break;
          default:
            break;
          }
        }
        break;
      case SDL_TEXTINPUT:
      {
        if (nInputFSM == 1)
        {
          strcat(szBuf, _event.text.text);
          printf("%s  \r", szBuf);
        }
      }
      break;
      case SDL_USEREVENT:
      {
        if (strcmp(_event.user.data1, "brush change") == 0)
        {
          g_nCurrentAttr = _event.user.code;
          printf("change bursh %d \n", g_nCurrentAttr);
        }
      }
      break;
      case SDL_QUIT:
        bLoop = SDL_FALSE;
        break;
      default:
        break;
      }
    }
  }

  SDL_DestroyTexture(g_pTileSet);

  tDE_closeCore(g_pEngineCore);

  SDL_Quit();
  return 0;
}
