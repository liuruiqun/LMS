<?php
/* @var $this WSNtableController */
/* @var $data WSNtable */
?>

<div class="view">

	<b><?php echo CHtml::encode($data->getAttributeLabel('NUMSEQ')); ?>:</b>
	<?php echo CHtml::link(CHtml::encode($data->NUMSEQ), array('view', 'id'=>$data->NUMSEQ)); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('NODEID')); ?>:</b>
	<?php echo CHtml::encode($data->NODEID); ?>
	<br />

<!--	<b>--><?php //echo CHtml::encode($data->getAttributeLabel('TYPE')); ?><!--:</b>-->
<!--	--><?php //echo CHtml::encode($data->TYPE); ?>
<!--	<br />-->

<!--	<b>--><?php //echo CHtml::encode($data->getAttributeLabel('TYPENOTE')); ?><!--:</b>-->
<!--	--><?php //echo CHtml::encode($data->TYPENOTE); ?>
<!--	<br />-->

	<b><?php echo CHtml::encode($data->getAttributeLabel('VALUE')); ?>:</b>
	<?php echo CHtml::encode($data->VALUE); ?>
	<br />

<!--	<b>--><?php //echo CHtml::encode($data->getAttributeLabel('SENDTIME')); ?><!--:</b>-->
<!--	--><?php //echo CHtml::encode($data->SENDTIME); ?>
<!--	<br />-->

	<b><?php echo CHtml::encode($data->getAttributeLabel('RECVTIME')); ?>:</b>
	<?php echo CHtml::encode($data->RECVTIME); ?>
	<br />

	<?php /*
	<b><?php echo CHtml::encode($data->getAttributeLabel('LOCALTION')); ?>:</b>
	<?php echo CHtml::encode($data->LOCALTION); ?>
	<br />

	*/ ?>

</div>