#ifndef _RENDERER_H_
#define _RENDERER_H_

/******************************************************************************
 * @name   renderer_init()
 * @brief  Initalises the renderer for use.
 * @return A value describing the success of the initalisation. 1 if successful
 *         0 if failed.
******************************************************************************/
int renderer_init();

/******************************************************************************
 * @name  renderer_deinit()
 * @brief Deinitalises the renderer.
******************************************************************************/
void renderer_deinit();

#endif /* _RENDERER_H_ */